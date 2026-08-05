#include "bsp_max30102.h"

#include "sw_iic_drive.h"
#include "STC32G_Delay.h"

// 传感器初始化标志
static uint8 g_max30102_inited = 0;

// 内部I2C读写函数 — 均返回 soft_iic 的 ACK 状态（0=成功, 非0=失败）
static uint8 max30102_readBytes(uint8 reg_addr, uint8 *buf, uint8 len)
{
    return soft_iic_read(MAX30102_I2C_ADDRESS, reg_addr, buf, len);
}

static uint8 max30102_writeByte(uint8 reg_addr, uint8 dat)
{
    return soft_iic_write(MAX30102_I2C_ADDRESS, reg_addr, &dat, 1);
}

/**
 * @brief 初始化MAX30102
 * @return 0-成功, 非0-失败
 */
uint8 max30102_init(void)
{
    uint8 res, part_id, prev, tmp;

    soft_i2c_init();
    // 检查器件ID
    res = max30102_readBytes(MAX30102_REG_PART_ID, &part_id, 1);
    if (res != 0 || part_id != 0x15)
    {
        return 1; // ID错误
    }

    // 软复位 (置位RST位)
    res = max30102_readBytes(MAX30102_REG_MODE_CONFIG, &prev, 1);
    if (res != 0)
        return 2;
    prev |= (1 << 6); // 设置RST位
    res = max30102_writeByte(MAX30102_REG_MODE_CONFIG, prev);
    if (res != 0)
        return 2;
    delay_ms(10); // 等待复位完成

    // 配置传感器工作模式、采样率、设置LED电流等
    // 例如: 采样率、使能中断等
    res = max30102_writeByte(MAX30102_REG_INTERRUPT_ENABLE_1, 0xE0);
    if (res != 0)
        return 3;
    res = max30102_writeByte(MAX30102_REG_INTERRUPT_ENABLE_2, 0x02);
    if (res != 0)
        return 4;

    // 复位FIFO (将读/写指针和溢出计数器清零)
    prev = 0x00;
    res = max30102_writeByte(MAX30102_REG_FIFO_READ_POINTER, prev);
    if (res != 0)
        return 5;
    res = max30102_writeByte(MAX30102_REG_FIFO_WRITE_POINTER, prev);
    if (res != 0)
        return 5;
    res = max30102_writeByte(MAX30102_REG_OVERFLOW_COUNTER, prev);
    if (res != 0)
        return 5;

    res = max30102_writeByte(MAX30102_REG_FIFO_CONFIG, 0x0F);
    if (res != 0)
        return 6;

    res = max30102_writeByte(MAX30102_REG_MODE_CONFIG, MAX30102_MODE_SPO2);
    if (res != 0)
        return 7;

    // SpO2 configuration:ACD resolution:15.63pA,sample rate control: 50Hz, LED pulse width:215 us
    res = max30102_writeByte(MAX30102_REG_SPO2_CONFIG, 0x23);
    if (res != 0)
        return 8;

    res = max30102_writeByte(MAX30102_REG_LED_PULSE_1, 0x3F);
    if (res != 0)
        return 9;
    res = max30102_writeByte(MAX30102_REG_LED_PULSE_2, 0x3F);
    if (res != 0)
        return 9;
    res = max30102_writeByte(MAX30102_REG_PILOT_LED_PULSE, 0x7f);
    if (res != 0)
        return 9;

    res = max30102_writeByte(MAX30102_REG_DIE_TEMP_CONFIG, 0x01); // temperture
    if (res != 0)
        return 10;

    // 清除任何挂起的中断
    max30102_readBytes(MAX30102_REG_INTERRUPT_STATUS_1, &tmp, 1);
    max30102_readBytes(MAX30102_REG_INTERRUPT_STATUS_2, &tmp, 1);

    g_max30102_inited = 1;
    return 0;
}

/**
 * @brief 从FIFO读取数据
 * @param raw_red [out] 红外通道原始值
 * @param raw_ir  [out] 红光通道原始值
 * @note 等待 PPG_RDY 位就绪，带超时保护；超时则保持上次值不变
 */
void max30102_readFIFO(uint32 *raw_red, uint32 *raw_ir)
{
    u8 receive_data[6] = {0};
    u8 temp_data = 0;
    u32 dat[2];
    u32 timeout = 3;
    u8 fifo_ready = 0;

    if (!raw_red || !raw_ir)
        return;

    /* 等待 FIFO 数据就绪（PPG_RDY 位），带超时 */
    do
    {
        max30102_readBytes(MAX30102_REG_INTERRUPT_STATUS_1, &temp_data, 1);
        if ((temp_data & 0x40) == 0x40)
        {
            fifo_ready = 1;
            break;
        }
        // 超时则放弃本次读取
        if (--timeout == 0)
            break;
    } while (1);

    if (!fifo_ready)
        return; // FIFO 无有效数据，保持上次值

    max30102_readBytes(MAX30102_REG_FIFO_DATA_REGISTER, receive_data, 6);
    dat[0] = ((receive_data[0] << 16) |
              (receive_data[1] << 8) |
              receive_data[2]) &
             0x03FFFF;

    dat[1] = ((receive_data[3] << 16) |
              (receive_data[4] << 8) |
              receive_data[5]) &
             0x03FFFF;
    *raw_red = dat[0];
    *raw_ir = dat[1];
}