#ifndef __BSP_DS18B20_H
#define __BSP_DS18B20_H

#include "type_def.h"

/* ============================================================
 * 1-Wire GPIO 引脚配置（解耦：修改宏即可换引脚）
 *
 * DS18B20_DQ_PORT       — 实际的 sfr 端口名（用于 C 代码中 |= / &= / & 操作）
 * DS18B20_DQ_PORT_NUM   — 端口数字编号（0~7，用于预处理器 #if 分支）
 * DS18B20_DQ_PIN_NUM    — 引脚位号（0~7）
 * DS18B20_DQ_PIN_MASK   — 引脚位掩码（1 << n）
 * ============================================================ */
#define DS18B20_DQ_PORT      P1
#define DS18B20_DQ_PORT_NUM  1
#define DS18B20_DQ_PIN_NUM   1
#define DS18B20_DQ_PIN_MASK  (1 << DS18B20_DQ_PIN_NUM)

/* ============================================================
 * DS18B20 ROM 命令
 * ============================================================ */
#define DS18B20_CMD_SEARCH_ROM 0xF0
#define DS18B20_CMD_READ_ROM 0x33
#define DS18B20_CMD_MATCH_ROM 0x55
#define DS18B20_CMD_SKIP_ROM 0xCC
#define DS18B20_CMD_ALARM_SEARCH 0xEC

/* ============================================================
 * DS18B20 功能命令
 * ============================================================ */
#define DS18B20_CMD_CONVERT_T 0x44
#define DS18B20_CMD_WRITE_SCRATCHPAD 0x4E
#define DS18B20_CMD_READ_SCRATCHPAD 0xBE
#define DS18B20_CMD_COPY_SCRATCHPAD 0x48
#define DS18B20_CMD_RECALL_E2 0xB8
#define DS18B20_CMD_READ_POWER_SUPPLY 0xB4

/* ============================================================
 * 温度分辨率配置
 * ============================================================ */
#define DS18B20_RES_9BIT 0x1F
#define DS18B20_RES_10BIT 0x3F
#define DS18B20_RES_11BIT 0x5F
#define DS18B20_RES_12BIT 0x7F

/* ============================================================
 * 返回值定义
 * ============================================================ */
#define DS18B20_OK 0
#define DS18B20_ERR_PRESENCE 1 // 未检测到存在脉冲
#define DS18B20_ERR_CRC 2      // CRC 校验失败

/* ============================================================
 * API 声明
 * ============================================================ */

/**
 * @brief 初始化 DS18B20（配置 GPIO 为开漏输出 + 上拉）
 */
void ds18b20_init(void);

/**
 * @brief 启动一次温度转换（异步，最大 750ms@12bit）
 * @param resolution 分辨率：DS18B20_RES_9BIT ~ DS18B20_RES_12BIT
 */
void ds18b20_start_conversion(u8 resolution);

/**
 * @brief 读取温度值（应在转换完成后调用）
 * @param celsius [out] 温度值（°C，精度 0.0625@12bit）
 * @return DS18B20_OK / DS18B20_ERR_PRESENCE / DS18B20_ERR_CRC
 */
u8 ds18b20_read_temperature(float *celsius);

#endif