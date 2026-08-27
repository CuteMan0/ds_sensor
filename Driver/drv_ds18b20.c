/**
 * @file    drv_ds18b20.c
 * @brief   DS18B20 数字温度传感器驱动（1-Wire 解耦实现）
 * @note    通过头文件宏定义即可切换 GPIO 引脚
 *          STC32G @ 24MHz
 */

#include "drv_ds18b20.h"

#include "stc32g.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

/* ============================================================
 * 1-Wire 底层宏（依赖头文件中的 DS18B20_DQ_PORT / DS18B20_DQ_PIN_MASK）
 *
 * 使用 STC32G_GPIO 库宏操作 GPIO 模式
 * 注意：#if 分支使用数字常量 DS18B20_DQ_PORT_NUM（0~7），
 *       因为 P0~P7 是 sfr 声明，预处理器无法求值。
 * ============================================================ */
#define DQ_HIGH() (DS18B20_DQ_PORT |= (DS18B20_DQ_PIN_MASK))
#define DQ_LOW() (DS18B20_DQ_PORT &= ~(DS18B20_DQ_PIN_MASK))
#define DQ_READ (DS18B20_DQ_PORT & (DS18B20_DQ_PIN_MASK))

/* GPIO 模式切换宏：用数字端口编号做预处理器分支 */
#if DS18B20_DQ_PORT_NUM == 0
#define DQ_OUT() P0_MODE_OUT_OD(DS18B20_DQ_PIN_MASK)
#define DQ_IN() P0_MODE_IN_HIZ(DS18B20_DQ_PIN_MASK)
#define DQ_INIT()                                \
    {                                            \
        P0_MODE_OUT_OD(DS18B20_DQ_PIN_MASK);     \
        P0_PULL_UP_ENABLE(DS18B20_DQ_PIN_MASK);  \
        P0_DIGIT_IN_ENABLE(DS18B20_DQ_PIN_MASK); \
    }
#elif DS18B20_DQ_PORT_NUM == 1
#define DQ_OUT() P1_MODE_OUT_OD(DS18B20_DQ_PIN_MASK)
#define DQ_IN() P1_MODE_IN_HIZ(DS18B20_DQ_PIN_MASK)
#define DQ_INIT()                                \
    {                                            \
        P1_MODE_OUT_OD(DS18B20_DQ_PIN_MASK);     \
        P1_PULL_UP_ENABLE(DS18B20_DQ_PIN_MASK);  \
        P1_DIGIT_IN_ENABLE(DS18B20_DQ_PIN_MASK); \
    }
#elif DS18B20_DQ_PORT_NUM == 2
#define DQ_OUT() P2_MODE_OUT_OD(DS18B20_DQ_PIN_MASK)
#define DQ_IN() P2_MODE_IN_HIZ(DS18B20_DQ_PIN_MASK)
#define DQ_INIT()                                \
    {                                            \
        P2_MODE_OUT_OD(DS18B20_DQ_PIN_MASK);     \
        P2_PULL_UP_ENABLE(DS18B20_DQ_PIN_MASK);  \
        P2_DIGIT_IN_ENABLE(DS18B20_DQ_PIN_MASK); \
    }
#elif DS18B20_DQ_PORT_NUM == 3
#define DQ_OUT() P3_MODE_OUT_OD(DS18B20_DQ_PIN_MASK)
#define DQ_IN() P3_MODE_IN_HIZ(DS18B20_DQ_PIN_MASK)
#define DQ_INIT()                                \
    {                                            \
        P3_MODE_OUT_OD(DS18B20_DQ_PIN_MASK);     \
        P3_PULL_UP_ENABLE(DS18B20_DQ_PIN_MASK);  \
        P3_DIGIT_IN_ENABLE(DS18B20_DQ_PIN_MASK); \
    }
#elif DS18B20_DQ_PORT_NUM == 4
#define DQ_OUT() P4_MODE_OUT_OD(DS18B20_DQ_PIN_MASK)
#define DQ_IN() P4_MODE_IN_HIZ(DS18B20_DQ_PIN_MASK)
#define DQ_INIT()                                \
    {                                            \
        P4_MODE_OUT_OD(DS18B20_DQ_PIN_MASK);     \
        P4_PULL_UP_ENABLE(DS18B20_DQ_PIN_MASK);  \
        P4_DIGIT_IN_ENABLE(DS18B20_DQ_PIN_MASK); \
    }
#elif DS18B20_DQ_PORT_NUM == 5
#define DQ_OUT() P5_MODE_OUT_OD(DS18B20_DQ_PIN_MASK)
#define DQ_IN() P5_MODE_IN_HIZ(DS18B20_DQ_PIN_MASK)
#define DQ_INIT()                                \
    {                                            \
        P5_MODE_OUT_OD(DS18B20_DQ_PIN_MASK);     \
        P5_PULL_UP_ENABLE(DS18B20_DQ_PIN_MASK);  \
        P5_DIGIT_IN_ENABLE(DS18B20_DQ_PIN_MASK); \
    }
#elif DS18B20_DQ_PORT_NUM == 6
#define DQ_OUT() P6_MODE_OUT_OD(DS18B20_DQ_PIN_MASK)
#define DQ_IN() P6_MODE_IN_HIZ(DS18B20_DQ_PIN_MASK)
#define DQ_INIT()                                \
    {                                            \
        P6_MODE_OUT_OD(DS18B20_DQ_PIN_MASK);     \
        P6_PULL_UP_ENABLE(DS18B20_DQ_PIN_MASK);  \
        P6_DIGIT_IN_ENABLE(DS18B20_DQ_PIN_MASK); \
    }
#elif DS18B20_DQ_PORT_NUM == 7
#define DQ_OUT() P7_MODE_OUT_OD(DS18B20_DQ_PIN_MASK)
#define DQ_IN() P7_MODE_IN_HIZ(DS18B20_DQ_PIN_MASK)
#define DQ_INIT()                                \
    {                                            \
        P7_MODE_OUT_OD(DS18B20_DQ_PIN_MASK);     \
        P7_PULL_UP_ENABLE(DS18B20_DQ_PIN_MASK);  \
        P7_DIGIT_IN_ENABLE(DS18B20_DQ_PIN_MASK); \
    }
#else
#error "DS18B20_DQ_PORT_NUM: unsupported port number (supported: 0~7)"
#endif

/* ============================================================
 * 1-Wire 精确延时（基于 STC32G_Delay.h 中的 delay_us）
 * ============================================================ */

/* 约 1μs 短延时 */
#define ow_delay_1us() delay_us(1)
/* 约 15μs 延时 */
#define ow_delay_15us() delay_us(15)
/* 约 45μs 延时 */
#define ow_delay_45us() delay_us(45)
/* 约 60μs 延时 */
#define ow_delay_60us() delay_us(60)
/* 约 480μs 延时 */
#define ow_delay_480us() delay_us(480)

/* ============================================================
 * 1-Wire 总线复位 & 存在脉冲检测
 * ============================================================ */
static u8 ow_reset(void)
{
    u8 presence;

    EA = 0; /* 复位时序不可被打断 */
    DQ_OUT();
    DQ_LOW();
    ow_delay_480us(); /* 拉低 480μs */
    DQ_HIGH();
    ow_delay_60us(); /* 释放总线 60μs */
    DQ_IN();
    presence = (DQ_READ == 0) ? 1 : 0; /* 0 = 器件应答（拉低） */
    EA = 1;                            /* 应答已采样，之后 480us 等可被打断 */
    ow_delay_480us();                  /* 等待完整复位时序结束 */

    return presence; /* 返回 1 = 检测到器件, 0 = 无器件 */
}

/* ============================================================
 * 1-Wire 位读写
 * ============================================================ */

/* 向总线写入 1 bit */
static void ow_write_bit(bit b)
{
    EA = 0; /* 保护 1-Wire 写 slot (~70μs) */
    DQ_OUT();
    DQ_LOW();
    ow_delay_1us(); /* 拉低 >1μs 表示写时序开始 */

    if (b)
        DQ_HIGH(); /* 写 1: 立即释放总线 */

    ow_delay_60us(); /* 主机至少保持/释放 60μs */
    DQ_HIGH();
    EA = 1;
    ow_delay_1us(); /* 恢复间隔 >1μs */
}

/* 从总线读取 1 bit */
static bit ow_read_bit(void)
{
    bit b;

    EA = 0; /* 保护 1-Wire 读 slot (~70μs) */
    DQ_OUT();
    DQ_LOW();
    ow_delay_1us(); /* 拉低 >1μs 表示读时序开始 */
    DQ_HIGH();
    ow_delay_1us(); /* 释放总线等待器件驱动 */

    DQ_IN();
    ow_delay_15us(); /* 15μs 内采样 */
    b = (DQ_READ != 0) ? 1 : 0;
    EA = 1;
    ow_delay_45us(); /* 等待完整读时序结束 */

    return b;
}

/* ============================================================
 * 1-Wire 字节读写
 * ============================================================ */

static void ow_write_byte(u8 byte)
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        ow_write_bit(byte & 0x01);
        byte >>= 1;
    }
}

static u8 ow_read_byte(void)
{
    u8 i, byte = 0;
    for (i = 0; i < 8; i++)
    {
        if (ow_read_bit())
            byte |= (1 << i);
    }
    return byte;
}

/* ============================================================
 * DS18B20 CRC8 校验（Dallas CRC-8: x^8 + x^5 + x^4 + 1）
 * ============================================================ */
static u8 ds18b20_crc8(const u8 *dat, u8 len)
{
    u8 crc = 0;
    u8 i, j;

    for (i = 0; i < len; i++)
    {
        u8 inbyte = dat[i];
        for (j = 0; j < 8; j++)
        {
            u8 mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix)
                crc ^= 0x8C; /* 多项式 0x31 反转 → 0x8C */
            inbyte >>= 1;
        }
    }
    return crc;
}

/* ============================================================
 * 公开 API 实现
 * ============================================================ */

/**
 * @brief 初始化 DS18B20 总线和 GPIO
 */
void ds18b20_init(void)
{
    /* 使用 STC32G_GPIO 库宏配置 DQ 引脚为准双向口 + 上拉 + 数字输入使能 */
    DQ_INIT();
    DQ_HIGH(); /* 总线空闲高电平 */
}

/**
 * @brief 启动一次温度转换（跳过 ROM 匹配，单器件总线）
 * @param resolution DS18B20_RES_9BIT ~ DS18B20_RES_12BIT
 */
void ds18b20_start_conversion(u8 resolution)
{
    /* 1. 复位并等待存在脉冲 */
    ow_reset();

    /* 2. SKIP ROM (0xCC) */
    ow_write_byte(DS18B20_CMD_SKIP_ROM);

    /* 3. 写入暂存器配置分辨率 */
    ow_write_byte(DS18B20_CMD_WRITE_SCRATCHPAD);
    ow_write_byte(0x00);       /* TH 寄存器（未使用） */
    ow_write_byte(0x00);       /* TL 寄存器（未使用） */
    ow_write_byte(resolution); /* 配置寄存器：分辨率 */

    /* 4. 复位 */
    ow_reset();

    /* 5. SKIP ROM (0xCC) */
    ow_write_byte(DS18B20_CMD_SKIP_ROM);

    /* 6. CONVERT T (0x44) — 启动温度转换 */
    ow_write_byte(DS18B20_CMD_CONVERT_T);
}

/**
 * @brief 读取温度值
 * @param celsius [out] 温度 °C
 * @return DS18B20_OK / DS18B20_ERR_PRESENCE / DS18B20_ERR_CRC
 */
u8 ds18b20_read_temperature(float *celsius)
{
    static u8 buf[9];
    u8 crc;

    if (!celsius)
        return DS18B20_ERR_PRESENCE;

    /* 1. 复位并检测存在脉冲 */
    if (!ow_reset())
        return DS18B20_ERR_PRESENCE;

    /* 2. SKIP ROM (0xCC) */
    ow_write_byte(DS18B20_CMD_SKIP_ROM);

    /* 3. READ SCRATCHPAD (0xBE) — 读取 9 字节暂存器 */
    ow_write_byte(DS18B20_CMD_READ_SCRATCHPAD);

    /* 4. 读取 9 字节 */
    {
        u8 i;
        for (i = 0; i < 9; i++)
            buf[i] = ow_read_byte();
    }

    /* 5. CRC 校验（暂存器前 8 字节的 CRC 应等于第 9 字节） */
    crc = ds18b20_crc8(buf, 8);
    if (crc != buf[8])
        return DS18B20_ERR_CRC;

    /* 6. 解析温度 */
    {
        int16 raw = (int16)((u16)buf[1] << 8 | buf[0]);
        *celsius = raw * 0.0625f;
    }

    return DS18B20_OK;
}
