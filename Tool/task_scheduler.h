#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__

#include "type_def.h"

/*
RTOS 风格协作式任务调度器 — 用法示例:

  task_scheduler_init();
  task_register(task_sensor, 3);   // 注册任务，指定优先级
  task_register(task_printf, 4);
  while(1) { task_scheduler_run(); }

  任务函数内部调用 task_delay_ms() 挂起自己:

    void task_sensor(void)
    {
        ds_update(&dat);
        task_delay_ms(50);  // 50ms 后再执行
    }

  Timer0 ISR (1ms):
    void Timer0_ISR(void) interrupt TMR0_VECTOR { task_scheduler_tick_isr(); }
*/

typedef void (*TaskFunc_t)(void);
typedef void (*TaskIdleHook_t)(void);

/* 任务统计结构 */
typedef struct
{
  u32 max_us;      // 单次最大执行时间(us)
  u32 min_us;      // 单次最小执行时间(us)
  u32 avg_us;      // 平均执行时间(us)
  u32 total_us;    // 累计执行时间(us)
  u32 exec_cnt;    // 执行次数
  u32 overrun_cnt; // 超时（积压）次数
  u32 last_dur_us; // 最近一次执行时间(us)
} TaskStats_t;

typedef struct
{
  u8 id;             // 任务ID
  u8 priority;       // 优先级（高值优先）
  u8 running;        // 是否正在执行（防止 task_delay_ms 在任务外被调用）
  u32 wake_tick;     // 下次唤醒的时间戳(ms)
  TaskFunc_t func;   // 任务函数
  TaskStats_t stats; // 运行时统计
} Task_t;

#define MAX_TASKS 8

typedef struct
{
  Task_t tasks[MAX_TASKS];
  u8 task_count;
  u8 current_task; // 当前正在执行的任务索引
  TaskIdleHook_t idle_hook;
} TaskScheduler_t;

/*========== 核心 API ==========*/
void task_scheduler_init(void);
u8 task_register(TaskFunc_t func, u8 priority); // 不再需要 period_ms
void task_scheduler_tick_isr(void);             // 放在 Timer0 ISR 中，1ms 调用一次
void task_scheduler_run(void);                  // 放在主循环 while(1) 中

/*========== 任务内部 API ==========*/

// 挂起当前任务 n 毫秒。只能在任务函数内调用，不能在任务外或 ISR 中调用。
// 调用后任务函数可以提前 return（也可以跑到末尾隐式返回）。
void task_delay_ms(u32 ms);

/*========== 工具 API ==========*/

// 设置空闲钩子（无任务就绪时调用，可用于喂狗 / 低功耗）
void task_set_idle_hook(TaskIdleHook_t hook);

// 原子读取系统毫秒滴答（可在中断上下文中安全使用）
u32 task_get_tick_ms(void);

// 非阻塞超时检查：返回 1=等待中，0=超时到
// 用法: static u32 tmo = 0; if (!task_timeout(&tmo, 500)) { ... }
// 注意：这是纯工具函数，不依赖调度器，可在任何地方使用
u8 task_timeout(u32 *t_next, u32 timeout_ms);

// 获取任务统计信息
const TaskStats_t *task_get_stats(u8 task_id);

/*========== 临界区 API（协议时序保护）==========

  当 UART / I2C / 1-Wire 等协议需要精确微秒级时序时，
  通过关中断来防止 1ms Timer0 ISR 打断时序。

  用法1 — 函数调用:
    u8 ea = task_enter_critical();
    ... 时序敏感操作（I2C start/stop, 1-Wire read slot 等）...
    task_exit_critical(ea);

  用法2 — 宏（零调用开销，推荐在 bsp 驱动中使用）:
    u8 _ea_;
    TASK_CRITICAL_ENTER(_ea_);
    ... 时序敏感操作 ...
    TASK_CRITICAL_EXIT(_ea_);
*/
u8 task_enter_critical(void);
void task_exit_critical(u8 ea_save);

#define TASK_CRITICAL_ENTER(ea) \
  do                            \
  {                             \
    (ea) = EA;                  \
    EA = 0;                     \
  } while (0)
#define TASK_CRITICAL_EXIT(ea) \
  do                           \
  {                            \
    EA = (ea);                 \
  } while (0)

#endif