#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__

#include "type_def.h"

/*
task_scheduler_init();
task_register(task, 1000, 1);

task_scheduler_run();

task_scheduler_tick_isr();
*/

typedef void (*TaskFunc_t)(void);

typedef struct
{
    u8 id;           // 任务ID
    u8 priority;     // 优先级（高值优先）
    u32 period_ms;   // 周期
    u32 last_tick;   // 上次执行时间
    TaskFunc_t func; // 任务函数
} Task_t;

#define MAX_TASKS 8

typedef struct
{
    Task_t tasks[MAX_TASKS];
    u8 task_count;
} TaskScheduler_t;

void task_scheduler_init(void);
u8 task_register(TaskFunc_t func, u32 period_ms, u8 priority);
void task_scheduler_tick_isr(void);
void task_scheduler_run(void);
u8 task_delay(u32 *t_next, u32 delay_ms);

#endif
