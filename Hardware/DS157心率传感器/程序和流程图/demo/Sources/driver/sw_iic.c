#include "sw_iic.h"

void soft_i2c_delay(void)
{
    delay_us(DELAY_TIME);
}

void soft_i2c_init(void)
{
    SDA_OUT();
    SCL_OUT();
    SDA_HIGH();
    SCL_HIGH();
}

void soft_i2c_start(void)
{
    SDA_OUT();
    SDA_HIGH();
    soft_i2c_delay();
    SCL_HIGH();
    soft_i2c_delay();
    SDA_LOW();
    soft_i2c_delay();

    SCL_LOW();
    soft_i2c_delay();
}

void soft_i2c_stop(void)
{
    SCL_LOW();
    soft_i2c_delay();
    SDA_OUT();
    SDA_LOW();
    soft_i2c_delay();
    SCL_HIGH();
    soft_i2c_delay();
    SDA_HIGH();
}

//i2c写入 返回1为ack，0为nack
bit soft_i2c_write_byte(u8 byte)
{
    u8 i;
    bit ack;
    SDA_OUT();
    for (i = 0; i < 8; i++) 
    {
        if (byte & 0x80)
            SDA_HIGH();
        else
            SDA_LOW();

        soft_i2c_delay();
        SCL_HIGH();
        soft_i2c_delay();
        SCL_LOW();
        byte <<= 1;
    }

    // 读取ACK
    SDA_IN();
    soft_i2c_delay();
    SCL_HIGH();
    ack = !(SDA_READ);
    soft_i2c_delay();
    SCL_LOW();
    SDA_OUT();

    return ack;
}

//i2c读出 参数写1为ack，写0为nack
u8 soft_i2c_read_byte(bit ack)
{
    u8 i;
    u8 byte = 0;
    SDA_IN();
    soft_i2c_delay();
    for (i = 0; i < 8; i++)
    {
        byte <<= 1;
        SCL_HIGH();
        soft_i2c_delay();
        if (SDA_READ)
            byte |= 0x01;
        SCL_LOW();
        soft_i2c_delay();
    }

    // 发出 ACK/NACK
    SDA_OUT();
    if (ack)
        SDA_LOW();
    else
        SDA_HIGH();

    soft_i2c_delay();
    SCL_HIGH();
    soft_i2c_delay();
    SCL_LOW();
    SDA_HIGH();

    return byte;
}

u8 soft_iic_read(u8 addr, u8 reg, u8 *buf, u16 len)
{
    u8 i;
    soft_i2c_start();
    soft_i2c_write_byte(addr<<1);
    soft_i2c_write_byte(reg);
    soft_i2c_start();
    soft_i2c_write_byte(addr<<1 | 0x01);
    for(i = 0;i < len-1;i++)
        *(buf+i) = soft_i2c_read_byte(1);
    *(buf+i) = soft_i2c_read_byte(0);
    soft_i2c_stop();
    return 0;
}

u8 soft_iic_write(u8 addr, u8 reg, u8 *buf, u16 len)
{
    u16 i;

    soft_i2c_start();

    if (!soft_i2c_write_byte(addr<<1))
        goto stop_fail;
    if (!soft_i2c_write_byte(reg))
        goto stop_fail;

    for (i = 0; i < len; i++)
    {
        if (!soft_i2c_write_byte(*(buf+i)))
            goto stop_fail;
    }

    soft_i2c_stop();
    return 0; // 写入成功

stop_fail:
    soft_i2c_stop();
    return 1; // 写入失败
}
