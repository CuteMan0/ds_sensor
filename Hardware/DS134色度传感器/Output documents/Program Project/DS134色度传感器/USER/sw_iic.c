#include "sw_iic.h"

#include "stc32g.h"
#include "STC32G_Delay.h"
#include "STC32G_GPIO.h"

void soft_i2c_delay(void)
{
    delay_us(5);
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
    soft_i2c_delay();
    SDA_OUT();
    SDA_HIGH();
    SCL_HIGH();
    soft_i2c_delay();
    SDA_LOW();
    soft_i2c_delay();
    SCL_LOW();
    soft_i2c_delay();
}

void soft_i2c_stop(void)
{
    SDA_OUT();
    SDA_LOW();
    soft_i2c_delay();
    SCL_HIGH();
    soft_i2c_delay();
    SDA_HIGH();
    soft_i2c_delay();
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