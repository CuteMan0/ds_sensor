#ifndef __DS131_PH_METER_H
#define __DS131_PH_METER_H

/**
 * @file    ds131_ph_meter.h
 * @brief   DS131 pH 传感器驱动头文件
 * @note    通过 ADC 采集 pH 探头电压，经分段线性校准模型计算 pH 值（范围 0~14），
 *          再经滑动平均滤波后通过 USB 输出。
 *          需要配合 ds_sensor.h 中的统一接口使用。
 */

#include "ds_sensor.h"

#if DS_SENSOR == 131

void ds_init(void);     /**< @brief 传感器初始化：配置 ADC 通道 0（参考电压 3.3V） */
void ds_update(void);   /**< @brief 数据采集与处理：ADC 采样 → 分段线性校准 → 限幅 → 滑动平均滤波 */
void ds_printf(void);   /**< @brief 格式化输出：通过 printf 输出 "ph:%.2f" */
void ds_calib(void);    /**< @brief 校准保存（当前预留，待实现） */

#endif
#endif