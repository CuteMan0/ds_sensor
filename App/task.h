#ifndef __TASK_H
#define __TASK_H

/**
 * @file    task.h
 * @brief   应用层任务函数声明
 * @note    各任务函数由 task_scheduler 调度器按注册的周期定时调用。
 */

/* ============================================================
 * 任务函数声明
 * ============================================================ */

/**
 * @brief 传感器数据打印任务
 * @note  调用 ds_printf() 格式化输出传感器数据，周期 4 ticks
 */
void task_printf(void);

/**
 * @brief 传感器校准保存任务
 * @note  调用 ds_calib() 执行校准保存，仅在触发校准后执行，周期 1 tick
 */
void task_calibration_save(void);

/**
 * @brief 按键扫描任务
 * @note  调用 Scan_Key() 检测按键输入，周期 2 ticks
 */
void task_key_scan(void);

/**
 * @brief LED 闪烁任务
 * @note  仅在 USE_BLINK 宏为真时生效，用于校准状态指示灯，周期 2 ticks
 */
void task_led_blink(void);

/**
 * @brief 传感器数据采集任务
 * @note  调用 ds_update() 执行传感器数据采集与滤波，无固定延时（由采集耗时决定）
 */
void task_sensor(void);

#endif /* __TASK_H */
