#include "drv_sht40.h"
#include "STC32G_Delay.h"
#include "sw_iic_drive.h"

/*===========================================================================
 * 内部类型定义
 *===========================================================================*/

typedef struct
{
    float humidity;
    float temperature;
} SHT20_data_t;

/*===========================================================================
 * 静态变量
 *===========================================================================*/

static SHT20_data_t g_sht40_data = {0.0f, 0.0f};

/*===========================================================================
 * 内部函数声明
 *===========================================================================*/

static u8 sht40_crc8(const u8 *dat, u8 len);
static void sht40_update(SHT20_data_t *h);

/*===========================================================================
 * 公开接口实现
 *===========================================================================*/

/**
 * @brief  SHT40 传感器初始化（含 I2C 初始化与传感器软复位）
 */
void SHT40_init(void)
{
    soft_i2c_init();
    soft_i2c_start();
    soft_i2c_write_byte(SHT40_I2C_ADDR << 1);
    soft_i2c_write_byte(SHT40_CMD_SOFT_RESET);
    soft_i2c_stop();
    delay_ms(50);
}

/**
 * @brief  读取土壤湿度值
 * @return 湿度百分比 (float, %RH)
 */
float SHT40_read_humidity(void)
{
    sht40_update(&g_sht40_data);
    return g_sht40_data.humidity;
}

/**
 * @brief  读取温度值
 * @return 温度值 (float, 单位: °C)
 */
float SHT40_read_temperature(void)
{
    sht40_update(&g_sht40_data);
    return g_sht40_data.temperature;
}

/*===========================================================================
 * 内部函数实现
 *===========================================================================*/

/**
 * @brief  SHT40 CRC-8 校验
 * @param  dat  数据指针
 * @param  len  数据长度
 * @return CRC 校验值
 */
static u8 sht40_crc8(const u8 *dat, u8 len)
{
    u8 crc = SHT40_CRC_INIT;
    u8 i, j;

    for (i = 0; i < len; i++)
    {
        crc ^= dat[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ SHT40_CRC_POLY;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/**
 * @brief  更新 SHT40 温湿度数据
 * @note   执行测量并读取结果，CRC 校验失败则丢弃
 * @param  h  数据存放指针
 */
static void sht40_update(SHT20_data_t *h)
{
    static u8 buf[6];
    u16 rawT, rawRH;
    u8 i = 0;

    // 发送测量命令
    soft_i2c_start();
    soft_i2c_write_byte(SHT40_I2C_ADDR << 1);
    soft_i2c_write_byte(SHT40_CMD_MEASURE_HIGH_PRECISION);

    // 等待测量完成
    delay_ms(10);

    // 读取6字节
    soft_i2c_start();
    soft_i2c_write_byte((SHT40_I2C_ADDR << 1) | 0x01);
    for (i = 0; i < 5; i++)
        buf[i] = soft_i2c_read_byte(1);
    buf[i] = soft_i2c_read_byte(0);
    soft_i2c_stop();

    // 校验 CRC
    if (sht40_crc8(buf, 2) != buf[2])
        return;
    if (sht40_crc8(buf + 3, 2) != buf[5])
        return;

    // 转换
    rawT = ((u16)buf[0] << 8) | buf[1];
    rawRH = ((u16)buf[3] << 8) | buf[4];

    h->temperature = -45.0f + 175.0f * ((float)rawT / 65535.0f);
    h->humidity = -6.0f + 125.0f * ((float)rawRH / 65535.0f);
}