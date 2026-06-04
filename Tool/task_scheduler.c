#include "task_scheduler.h"

#include "STC32G_Timer.h"

static TaskScheduler_t scheduler;
static volatile u32 sys_tick_ms = 0;

//------------------------------------------------------------
// 初始化任务调度器
//------------------------------------------------------------
void task_scheduler_init(void)
{
    scheduler.task_count = 0;
    sys_tick_ms = 0;

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
// 主循环中调用，调度任务执行
//------------------------------------------------------------
void task_scheduler_run(void)
{
    u8 i, j;
    Task_t *ready_list[MAX_TASKS];
    u8 ready_count = 0;
    u32 now;

    EA = 0;
    now = sys_tick_ms;
    EA = 1;

    for (i = 0; i < scheduler.task_count; i++)
    {
        Task_t *t = &scheduler.tasks[i];
        if (now - t->last_tick >= t->period_ms)
        {
            t->last_tick += t->period_ms;
            ready_list[ready_count++] = t;
        }
    }

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

    for (i = 0; i < ready_count; i++)
    {
        ready_list[i]->func();
    }
}
