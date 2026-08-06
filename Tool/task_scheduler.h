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
typedef void (*TaskIdleHook_t)(void);

/* 任务统计结构 */
typedef struct
{
    u32 max_us; // 单次最大执行时间(us)
    u32 min_us; // 单次最小执行时间(us)
    u32 avg_us; // 平均执行时间(us)
    u32 total_us; // 累计执行时间(us)
    u32 exec_cnt; // 执行次数
    u32 overrun_cnt; // 超时（积压）次数
    u32 last_dur_us; // 最近一次执行时间(us)
} TaskStats_t;

typedef struct
{
    u8 id;        // 任务ID
    u8 priority;  // 优先级（高值优先）
    u32 period_ms; // 周期(ms)
    u32 last_tick; // 上次执行时的 tick
    TaskFunc_t func; // 任务函数
    TaskStats_t stats; // 运行时统计
} Task_t;

#define MAX_TASKS 8

typedef struct
{
    Task_t tasks[MAX_TASKS];
    u8 task_count;
    TaskIdleHook_t idle_hook; // 空闲钩子（喂狗/低功耗）
} TaskScheduler_t;

/*--------- 核心 API ---------*/
void task_scheduler_init(void);
u8 task_register(TaskFunc_t func, u32 period_ms, u8 priority);
void task_scheduler_tick_isr(void);
void task_scheduler_run(void);

/*--------- 工具 API ---------*/
// 设置空闲钩子（在调度器无任务执行时调用，可用于喂狗）
void task_set_idle_hook(TaskIdleHook_t hook);

// 原子读取系统毫秒滴答（可在中断上下文中安全使用）
u32 task_get_tick_ms(void);

// 获取系统启动以来的微秒时间戳（用于非阻塞延迟）
u32 task_get_tick_us(void);

// 非阻塞延迟：返回 1=等待中，0=超时到
// 用法: static u32 t_next=0; if(!task_delay_us(&t_next, 500)) { ... }
u8 task_delay_us(u32 *t_next, u32 delay_us);

// 获取任务统计信息
const TaskStats_t *task_get_stats(u8 task_id);

// 非阻塞毫秒延迟（兼容旧 API，内部推荐用 task_delay_us）
u8 task_delay(u32 *t_next, u32 delay_ms);

#endif