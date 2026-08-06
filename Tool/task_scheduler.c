#include "task_scheduler.h"

#include "config.h"

#define TIMER0_RELOAD 0xA240 // 1ms@24MHz, 1T mode

static TaskScheduler_t scheduler;
static volatile u32 sys_tick_ms = 0;
static volatile u32 sys_tick_us = 0; // ISR 中 += 1000

//------------------------------------------------------------
// 初始化任务调度器
//------------------------------------------------------------
void task_scheduler_init(void)
{
    u8 i;
    scheduler.task_count = 0;
    scheduler.current_task = 0xFF; // 无任务在运行
    scheduler.idle_hook = 0;
    sys_tick_ms = 0;
    sys_tick_us = 0;

    for (i = 0; i < MAX_TASKS; i++)
    {
        scheduler.tasks[i].stats.max_us = 0;
        scheduler.tasks[i].stats.min_us = 0xFFFFFFFF;
        scheduler.tasks[i].stats.avg_us = 0;
        scheduler.tasks[i].stats.total_us = 0;
        scheduler.tasks[i].stats.exec_cnt = 0;
        scheduler.tasks[i].stats.overrun_cnt = 0;
        scheduler.tasks[i].stats.last_dur_us = 0;
        scheduler.tasks[i].running = 0;
        scheduler.tasks[i].wake_tick = 0; // 0 表示立即可调度
    }

    // 1ms@24.000MHz
    TM0PS = 0x00; // Set timer clock prescaler
    AUXR |= 0x80; // Timer clock is 1T mode
    TMOD &= 0xF0; // Set timer work mode
    TL0 = (u8)(TIMER0_RELOAD & 0xFF);
    TH0 = (u8)(TIMER0_RELOAD >> 8);
    TF0 = 0; // Clear TF0 flag
    TR0 = 1; // Timer0 start run
    ET0 = 1; // Enable timer0 interrupt
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
// 注册一个任务（RTOS 风格，不需要 period_ms）
//------------------------------------------------------------
u8 task_register(TaskFunc_t func, u8 priority)
{
    u8 id = scheduler.task_count;
    if (id >= MAX_TASKS)
        return 0xFF;

    scheduler.tasks[id].id = id;
    scheduler.tasks[id].priority = priority;
    scheduler.tasks[id].wake_tick = 0; // 首次立即可调度
    scheduler.tasks[id].running = 0;
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
    sys_tick_us += 1000;
}

//------------------------------------------------------------
// 挂起当前任务 n 毫秒（只能在任务函数内调用）
//------------------------------------------------------------
void task_delay_ms(u32 ms)
{
    u8 id = scheduler.current_task;
    if (id >= scheduler.task_count)
        return; // 安全保护：不在任务上下文中调用则忽略

    scheduler.tasks[id].wake_tick = task_get_tick_ms() + ms;
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
// 非阻塞超时检查
// 返回: 1=等待中, 0=超时到
// 用法: static u32 tmo = 0; if (!task_timeout(&tmo, 500)) { ... }
//------------------------------------------------------------
u8 task_timeout(u32 *t_next, u32 timeout_ms)
{
    u32 now = task_get_tick_ms();

    if (*t_next == 0)
    {
        *t_next = now + timeout_ms;
        return 1;
    }

    // 无符号回绕安全比较：now < t_next
    if ((u32)(now - *t_next) >= 0x80000000UL)
        return 1;

    *t_next = now + timeout_ms;
    return 0;
}

//------------------------------------------------------------
// 进入临界区：关中断，保存并返回 EA 状态
//------------------------------------------------------------
u8 task_enter_critical(void)
{
    u8 s = EA;
    EA = 0;
    return s;
}

//------------------------------------------------------------
// 退出临界区：恢复 EA 状态
//------------------------------------------------------------
void task_exit_critical(u8 ea_save)
{
    EA = ea_save;
}

//------------------------------------------------------------
// 主循环中调用，调度任务执行
//------------------------------------------------------------
void task_scheduler_run(void)
{
    u8 i, j;
    Task_t *ready_list[MAX_TASKS];
    u8 ready_count = 0;
    u32 now, now_us, start_us, dur_us;

    now = task_get_tick_ms();

    // 扫描就绪任务：wake_tick <= now
    for (i = 0; i < scheduler.task_count; i++)
    {
        Task_t *t = &scheduler.tasks[i];

        // 无符号回绕安全：now >= wake_tick 等价于 (now - wake_tick) < 0x80000000UL
        if ((u32)(now - t->wake_tick) < 0x80000000UL)
        {
            // 积压检测：wake_tick 落后超过 1ms 说明被延迟了
            // （注意：这里只检测“就绪后有没有被调度延迟”，不是周期积压）
            if ((u32)(now - t->wake_tick) > 1)
            {
                t->stats.overrun_cnt++;
            }
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

    // 执行就绪任务并统计耗时（精度 1ms）
    for (i = 0; i < ready_count; i++)
    {
        Task_t *t = ready_list[i];

        // 标记任务正在运行，记录当前任务 ID 给 task_delay_ms 使用
        t->running = 1;
        scheduler.current_task = t->id;

        EA = 0;
        start_us = sys_tick_us;
        EA = 1;

        t->func(); // 任务内可能调用 task_delay_ms() 设置 wake_tick

        EA = 0;
        now_us = sys_tick_us;
        EA = 1;

        dur_us = now_us - start_us;

        t->running = 0;

        // 更新统计
        t->stats.last_dur_us = dur_us;
        t->stats.total_us += dur_us;
        t->stats.exec_cnt++;
        if (dur_us > t->stats.max_us)
            t->stats.max_us = dur_us;
        if (dur_us < t->stats.min_us)
            t->stats.min_us = dur_us;
        t->stats.avg_us = t->stats.total_us / t->stats.exec_cnt;
    }

    scheduler.current_task = 0xFF; // 无任务运行

    // 无任务就绪时调用空闲钩子
    if (ready_count == 0 && scheduler.idle_hook)
    {
        scheduler.idle_hook();
    }
}