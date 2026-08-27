#ifndef __DRV_FORCE_SENSOR_H__
#define __DRV_FORCE_SENSOR_H__

#include "type_def.h"

/**
 * @brief  力传感器初始化
 * @note   从EEPROM读取校准数据，并初始化ADC0/ADC1通道的
 *         偏移和满量程校准值
 */
void FORCE_SENSOR_init(void);

/**
 * @brief  力传感器偏移/满量程校准设置
 * @param  padc: ADC采样缓冲指针
 * @param  mode: 校准模式
 *            0 - 仅累加偏移量，不保存到EEPROM
 *            1 - 满量程校准，计算比例系数
 *            2 - 从EEPROM读取偏移和满量程校准值
 *            3 - 保存偏移到EEPROM
 */
void FORCE_SENSOR_set_calib(ADC_SampleBuffer *padc, u8 mode);

/**
 * @brief  力传感器标定处理流程
 * @note   当标定标志置位时，等待ADC平均值稳定后
 *         自动保存满量程校准值并关闭LED显示
 * @param  padc: ADC采样缓冲指针
 */
void FORCE_SENSOR_calib_proc(ADC_SampleBuffer *padc);

/**
 * @brief  触发力传感器标定
 * @note   置位内部标定标志，配合 FORCE_SENSOR_calib_proc() 使用
 */
void FORCE_SENSOR_trigger_calib(void);

#endif /* __BSP_FORCE_SENSOR_H__ */