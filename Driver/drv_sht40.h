#ifndef __DRV_SHT40_H__
#define __DRV_SHT40_H__

#include "type_def.h"

/* SHT40 7-bit I2C 地址 */
#define SHT40_I2C_ADDR                     (0x44)

/* SHT40 命令 */
#define SHT40_CMD_MEASURE_HIGH_PRECISION   (0xFD)
#define SHT40_CMD_SOFT_RESET               (0x94)

/* SHT40 CRC 校验参数 */
#define SHT40_CRC_POLY                     (0x31)
#define SHT40_CRC_INIT                     (0xFF)

/**
 * @brief  SHT40 传感器初始化（含 I2C 初始化与传感器软复位）
 */
void SHT40_init(void);

/**
 * @brief  读取土壤湿度值
 * @return 湿度百分比 (float, %RH)
 */
float SHT40_read_humidity(void);

/**
 * @brief  读取温度值
 * @return 温度值 (float, 单位: °C)
 */
float SHT40_read_temperature(void);

#endif /* __DRV_SHT40_H__ */