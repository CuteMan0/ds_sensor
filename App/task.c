/**
 * @file    task.c
 * @brief   应用层任务函数实现
 * @note    包含传感器打印、校准保存、按键扫描、LED 闪烁和传感器采集任务。
 *          所有任务由 task_scheduler 调度器按周期定时调用。
 */

#include "task.h"

#include "ds_sensor.h"

#include "led_drive.h"
#include "key_drive.h"

/* ============================================================
 * 全局变量定义（由 ds_sensor.h 中的 extern 声明引用）
 * ============================================================ */

volatile float dat_for_printf; /**< 待打印的传感器数据（由传感器驱动更新） */

char usb_txbuf[64];   /**< USB 发送缓冲区 */
u8 usb_send_flag = 0; /**< USB 发送完成标志，1=发送完成 */
u8 usb_send_len = 0;  /**< USB 发送数据长度（字节数） */

/* ============================================================
 * 任务函数实现
 * ============================================================ */

/**
 * @brief 传感器数据打印任务
 * @note  周期调用 ds_printf() 输出传感器数据，每次执行后延时 10ms
 */
void task_printf(void)
{
    ds_printf();
#if DS_SENSOR == 158 //ecg 需要高采样输出
    task_delay_ms(1);
#else
    task_delay_ms(100);
#endif
}

/**
 * @brief 传感器校准保存任务
 * @note  周期 1s 调用 ds_calib()，仅在触发校准条件时执行实际校准操作
 */
void task_calibration_save(void)
{
    ds_calib(); /* 统一校准接口，仅在触发校准条件后执行 */
    task_delay_ms(1000);
}

/**
 * @brief 按键扫描任务
 * @note  周期 20ms 调用 Scan_Key() 检测按键输入
 */
void task_key_scan(void)
{
    Scan_Key();
    task_delay_ms(20);
}

/**
 * @brief LED 闪烁任务
 * @note  由 USE_BLINK 宏控制是否启用，用于校准状态指示灯
 */
void task_led_blink(void)
{
#if USE_BLINK
    Led_Task();
#endif
    task_delay_ms(10);
}

/**
 * @brief 传感器数据采集任务
 * @note  调用 ds_update() 统一传感器更新接口，内部完成数据采集 + 滤波 + 超时处理
 */
void task_sensor(void)
{
    ds_update(); /* 统一传感器更新接口，内部完成采集 + 滤波 + 超时处理 */
}
