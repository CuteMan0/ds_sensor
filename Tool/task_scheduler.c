#include "task_scheduler.h"

#include "STC32G_Timer.h"

static TaskScheduler_t scheduler;
static volatile u32 sys_tick_ms = 0;

//------------------------------------------------------------
// 初始化任务调度器
//------------------------------------------------------------
void task_scheduler_init(void)
{
    u8 i;
    scheduler.task_count = 0;
    scheduler.idle_hook = 0;
    sys_tick_ms = 0;

    for (i = 0; i < MAX_TASKS; i++)
    {
        scheduler.tasks[i].stats.max_us = 0;
        scheduler.tasks[i].stats.min_us = 0xFFFFFFFF;
        scheduler.tasks[i].stats.avg_us = 0;
        scheduler.tasks[i].stats.total_us = 0;
        scheduler.tasks[i].stats.exec_cnt = 0;
        scheduler.tasks[i].stats.overrun_cnt = 0;
        scheduler.tasks[i].stats.last_dur_us = 0;
    }

    // 1ms@24.000MHz
    TM0PS = 0x00; // Set timer clock prescaler
    AUXR |= 0x80; // imer clock is 1T mode
    TMOD &= 0xF0; // Set timer work mode
    TL0 = 0x40;   // Initial timer value
    TH0 = 0xA2;   // Initial timer value
    TF0 = 0;      // Clear TF0 flag
    TR0 = 1;      // Timer0 start run
    ET0 = 1;      // Enable timer0 interrupt
}

//------------------------------------------------------------
// 设置空闲钩子
//------------------------------------------------------------
void task_set_idle_hook(TaskIdleHook_t hook)
{
    scheduler.idle_hook = hook;
}

//------------------------------------------------------------
// 原子读取系统滴答（关闭 EA 保证 u32 读原子性）
//------------------------------------------------------------
u32 task_get_tick_ms(void)
{
    u32 t;
    EA = 0;
    t = sys_tick_ms;
    EA = 1;
    return t;
}

//------------------------------------------------------------
// 获取微秒级时间戳（基于 ms tick + Timer0 当前计数值）
// 精度：约 1us（24MHz 1T 模式）
//------------------------------------------------------------
u32 task_get_tick_us(void)
{
    u32 t_ms, elapsed_ticks;
    u16 t0_val;
    u8 ea_save = EA;

    EA = 0;
    t_ms = sys_tick_ms;
    // 读取 Timer0 当前计数值（16位自动重装，从 TL0/TH0 到 0xFFFF 溢出）
    t0_val = (TH0 << 8) | TL0;
    // 检查是否在读取期间发生溢出
    if (TF0)
    {
        // 溢出已发生但 ISR 尚未执行，补偿 1ms
        t0_val = (TH0 << 8) | TL0;
        if ((u16)(0xA240 - t0_val) > 0xA240 / 2)
        {
            // 确实刚溢出，补偿
            t_ms++;
        }
    }
    EA = ea_save;

    // 计算从重装值到当前值的 ticks（向下计数）
    // 重装值 ≈ 0xA240，满值 0xFFFF，每个 tick = 1/24 μs ≈ 0.0417μs
    // 简化：ticks 直接对应 us 近似值
    elapsed_ticks = (u16)(t0_val - 0xA240);
    // 24MHz 1T: 1 tick = 1/24 us.  elapsed_ticks / 24 = elapsed_us
    return t_ms * 1000 + elapsed_ticks / 24;
}

//------------------------------------------------------------
// 注册一个周期任务
//------------------------------------------------------------
u8 task_register(TaskFunc_t func, u32 period_ms, u8 priority)
{
    u8 id = scheduler.task_count;
    if (id >= MAX_TASKS)
        return 0xFF;

    scheduler.tasks[id].id = id;
    scheduler.tasks[id].priority = priority;
    scheduler.tasks[id].period_ms = period_ms;
    scheduler.tasks[id].last_tick = 0;
    scheduler.tasks[id].func = func;

    scheduler.task_count++;
    return id;
}

//------------------------------------------------------------
// 中断调用，系统时基更新
//------------------------------------------------------------
void task_scheduler_tick_isr(void)
{
    sys_tick_ms++;
}

//------------------------------------------------------------
// 获取任务统计
//------------------------------------------------------------
const TaskStats_t *task_get_stats(u8 task_id)
{
    if (task_id >= scheduler.task_count)
        return 0;
    return &scheduler.tasks[task_id].stats;
}

//------------------------------------------------------------
// 非阻塞微秒延迟
// 返回: 1=还在等待，0=超时已到
//------------------------------------------------------------
u8 task_delay_us(u32 *t_next, u32 delay_us)
{
    u32 now = task_get_tick_us();

    if (*t_next == 0)
    {
        *t_next = now + delay_us;
        return 1;
    }

    // 纯 u32 比较，利用无符号回绕
    if ((u32)(now - *t_next) >= 0x80000000UL) // now < t_next 等价
        return 1;                              // 还没到

    *t_next = now + delay_us;
    return 0; // 超时到达
}

//------------------------------------------------------------
// 非阻塞毫秒延迟（兼容旧 API）
//------------------------------------------------------------
u8 task_delay(u32 *t_next, u32 delay_ms)
{
    u32 now = task_get_tick_ms();

    if (*t_next == 0)
    {
        *t_next = now + delay_ms;
        return 1;
    }

    if ((u32)(now - *t_next) >= 0x80000000UL)
        return 1;

    *t_next = now + delay_ms;
    return 0;
}

//------------------------------------------------------------
// 主循环中调用，调度任务执行
//------------------------------------------------------------
void task_scheduler_run(void)
{
    u8 i, j;
    Task_t *ready_list[MAX_TASKS];
    u8 ready_count = 0;
    u32 now, now_us, start_us, dur_us, elapsed;

    now = task_get_tick_ms();

    // 扫描到期任务
    for (i = 0; i < scheduler.task_count; i++)
    {
        Task_t *t = &scheduler.tasks[i];

        // 首次调度：last_tick==0 时直接触发
        if (t->last_tick == 0)
        {
            t->last_tick = now;
            ready_list[ready_count++] = t;
            continue;
        }

        // 时间到？
        elapsed = now - t->last_tick;
        if (elapsed >= t->period_ms)
        {
            // 积压检测：如果流逝时间超过 2 倍周期，说明任务被严重延迟
            if (elapsed >= t->period_ms * 2)
            {
                t->stats.overrun_cnt++;
            }

            // 重置 last_tick 为 now，放弃追赶，避免连续多次执行
            t->last_tick = now;
            ready_list[ready_count++] = t;
        }
    }

    // 按优先级排序（高值优先，冒泡）
    for (i = 0; i < ready_count; i++)
    {
        for (j = i + 1; j < ready_count; j++)
        {
            if (ready_list[j]->priority > ready_list[i]->priority)
            {
                Task_t *tmp = ready_list[i];
                ready_list[i] = ready_list[j];
                ready_list[j] = tmp;
            }
        }
    }

    // 执行就绪任务并统计耗时
    for (i = 0; i < ready_count; i++)
    {
        Task_t *t = ready_list[i];
        start_us = task_get_tick_us();
        t->func();
        now_us = task_get_tick_us();
        dur_us = now_us - start_us;

        // 更新统计
        t->stats.last_dur_us = dur_us;
        t->stats.total_us += dur_us;
        t->stats.exec_cnt++;
        if (dur_us > t->stats.max_us)
            t->stats.max_us = dur_us;
        if (dur_us < t->stats.min_us)
            t->stats.min_us = dur_us;
        // avg 通过 total/exec_cnt 计算即可
    }

    // 无任务就绪时调用空闲钩子
    if (ready_count == 0 && scheduler.idle_hook)
    {
        scheduler.idle_hook();
    }
}