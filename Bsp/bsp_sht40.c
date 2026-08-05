/*===========================================================================
 * @file        bsp_sht40.c
 * @brief       SHT40 温湿度传感器 BSP 驱动
 * @attention   基于 I2C 通信，适用于 SHT40 系列传感器
 *===========================================================================*/

#include "bsp_sht40.h"
#include "sw_iic_drive.h"
#include "STC32G_Delay.h"

/*===========================================================================
 * 内部类型定义
 *===========================================================================*/

typedef struct
{
    float humidity;    /* 湿度 (%RH) */
    float temperature; /* 温度 (°C)   */
} bsp_sht40_data_t;

/*===========================================================================
 * 静态变量
 *===========================================================================*/

static bsp_sht40_data_t g_sht40_data = {0.0f, 0.0f};

/*===========================================================================
 * 内部函数声明
 *===========================================================================*/

static void bsp_sht40_send_cmd(u8 cmd);
static void bsp_sht40_read_data(u8 *buf, u8 len);
static u8 bsp_sht40_crc8(const u8 *dat, u8 len);
static void bsp_sht40_update(void);

/*===========================================================================
 * 公开接口实现
 *===========================================================================*/

/**
 * @brief  SHT40 传感器初始化
 */
void bsp_sht40_init(void)
{
    soft_i2c_init();
    bsp_sht40_send_cmd(BSP_SHT40_CMD_SOFT_RESET);
    delay_ms(50);
}

/**
 * @brief  读取土壤湿度
 * @return 湿度百分比 (%RH)
 */
float bsp_sht40_read_humidity(void)
{
    bsp_sht40_update();
    return g_sht40_data.humidity;
}

/**
 * @brief  读取温度
 * @return 温度值 (°C)
 */
float bsp_sht40_read_temperature(void)
{
    bsp_sht40_update();
    return g_sht40_data.temperature;
}

/*===========================================================================
 * 内部函数实现
 *===========================================================================*/

/**
 * @brief  向 SHT40 发送单字节命令
 * @param  cmd  命令字节
 */
static void bsp_sht40_send_cmd(u8 cmd)
{
    soft_i2c_start();
    soft_i2c_write_byte(BSP_SHT40_I2C_ADDR << 1);
    soft_i2c_write_byte(cmd);
    soft_i2c_stop();
}

/**
 * @brief  从 SHT40 读取指定长度数据
 * @param  buf  数据缓冲区
 * @param  len  读取长度
 */
static void bsp_sht40_read_data(u8 *buf, u8 len)
{
    u8 i;

    soft_i2c_start();
    soft_i2c_write_byte((BSP_SHT40_I2C_ADDR << 1) | 0x01);

    for (i = 0; i < len - 1; i++)
    {
        buf[i] = soft_i2c_read_byte(1); /* 发送 ACK  */
    }
    buf[i] = soft_i2c_read_byte(0); /* 发送 NACK */

    soft_i2c_stop();
}

/**
 * @brief  SHT40 CRC-8 校验
 * @param  dat  数据指针
 * @param  len  数据长度
 * @return CRC 校验值
 */
static u8 bsp_sht40_crc8(const u8 *dat, u8 len)
{
    u8 crc = BSP_SHT40_CRC_INIT;
    u8 i, j;

    for (i = 0; i < len; i++)
    {
        crc ^= dat[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ BSP_SHT40_CRC_POLY;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief  更新 SHT40 温湿度数据
 * @note   执行测量并读取结果，CRC 校验失败则丢弃
 */
static void bsp_sht40_update(void)
{
    u8 buf[6];
    u16 raw_temp;
    u16 raw_humi;

    /* 1. 发送高精度测量命令 */
    bsp_sht40_send_cmd(BSP_SHT40_CMD_MEASURE_HIGH_PRECISION);

    /* 2. 等待测量完成（典型值 8.3ms，取 10ms 留余量） */
    delay_ms(10);

    /* 3. 读取 6 字节数据（温湿度各 2 字节 + 各 1 字节 CRC） */
    bsp_sht40_read_data(buf, 6);

    /* 4. CRC 校验 */
    if (bsp_sht40_crc8(buf, 2) != buf[2])
        return; /* 温度 CRC 错误 */
    if (bsp_sht40_crc8(buf + 3, 2) != buf[5])
        return; /* 湿度 CRC 错误 */

    /* 5. 数据转换 */
    raw_temp = ((u16)buf[0] << 8) | buf[1];
    raw_humi = ((u16)buf[3] << 8) | buf[4];

    g_sht40_data.temperature = -45.0f + 175.0f * ((float)raw_temp / 65535.0f);
    g_sht40_data.humidity = -6.0f + 125.0f * ((float)raw_humi / 65535.0f);
}