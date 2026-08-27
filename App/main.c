/**
 * @file    main.c
 * @brief   传感器系统主程序
 * @note    系统主频 24MHz，STC32G 平台。
 *          初始化流程：STC 系统 → 传感器 → 工具 → 任务调度器 → 注册任务 → 运行调度器。
 *          Timer0 为任务调度器提供时基中断（1ms tick）。
 */

//------------------------------ STC 库 ------------------------------
#include "stc32g.h"

//------------------------------ BSP 库 ------------------------------

//------------------------------ 任务与传感器 ------------------------------
#include "ds_sensor.h"
#include "task.h"

/* ============================================================
 * 全局变量定义
 * ============================================================ */

/** @brief 滑动平均滤波器实例 */
avg_filter_t filter;

/** @brief 滤波数据缓冲区，NUM_BUF_AVG 定义在 ds_sensor.h */
avgf_data_t buffer[NUM_BUF_AVG];

/* ============================================================
 * 系统初始化
 * ============================================================ */

/**
 * @brief STC 系统初始化
 * @note  配置系统时钟（24MHz）、USB 通信、LED 显示和按键 GPIO。
 *        开启全局中断 EA。
 */
void STC_init(void)
{
    WTST = 0;  /* 零等待状态 */
    EAXFR = 1; /* 使能扩展 XFR 访问 */
    CKCON = 0; /* 系统时钟不分频 */

    usb_init(); /* USB CDC 初始化 */
    EA = 1;     /* 开启全局中断 */

    DIS_LED_init();  /* LED 显示初始化 */
    KEY_GPIO_init(); /* 按键 GPIO 初始化 */
}

/**
 * @brief 工具模块初始化
 * @note  初始化滑动平均滤波器。
 */
void Core_init(void)
{
    avg_filter_init(&filter, buffer, NUM_BUF_AVG);
}

/* ============================================================
 * 主函数
 * ============================================================ */

/**
 * @brief 主函数入口
 * @note  初始化顺序：STC_init → ds_init → Core_init → task_scheduler_init。
 *        注册 5 个周期任务后进入主循环，由 task_scheduler_run() 调度执行。
 *
 *        任务注册表：
 *          - task_printf           : 周期 4 ticks，传感器数据打印
 *          - task_sensor           : 周期 3 ticks，传感器数据采集
 *          - task_calibration_save : 周期 1 tick， 校准保存
 *          - task_key_scan         : 周期 2 ticks，按键扫描
 *          - task_led_blink        : 周期 2 ticks，LED 闪烁
 */
void main(void)
{
    STC_init();
    ds_init(); /* 传感器初始化（由 dsXX 驱动文件实现） */
    Core_init();
    task_scheduler_init(); /* 任务调度器初始化 */

    /* 注册周期任务 */
    task_register(task_printf, 4);
    task_register(task_sensor, 3);
    task_register(task_calibration_save, 1);
    task_register(task_key_scan, 2);
    task_register(task_led_blink, 2);

    /* 主循环：任务调度器运行 */
    while (1)
    {
        task_scheduler_run();
    }
}

/* ============================================================
 * 中断服务函数
 * ============================================================ */

/**
 * @brief Timer0 中断服务函数
 * @note  为任务调度器提供 1ms 时基，调用 task_scheduler_tick_isr() 更新系统 tick。
 */
void Timer0_ISR_Handler(void) interrupt TMR0_VECTOR
{
    task_scheduler_tick_isr(); /* 任务调度器时基更新 */
}
