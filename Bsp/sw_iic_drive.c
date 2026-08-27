#include "sw_iic_drive.h" 
 
#include "stc32g.h" 
#include "STC32G_Delay.h" 
#include "STC32G_GPIO.h" 
 
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
 
// i2c写入 返回1为ack，0为nack 
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
 
// i2c读出 参数写1为ack，写0为nack 
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
    u16 i; 
    u8 ret = 1; 
 
    if (!buf || len == 0) 
        return 1; 
 
    /* 整个I2C读操作进入临界区 */ 
    EA = 0; 
 
    /* 发送设备地址（写）并检查 ACK */ 
    soft_i2c_start(); 
    if (!soft_i2c_write_byte(addr << 1)) 
        goto read_fail; 
 
    /* 发送寄存器地址并检查 ACK */ 
    if (!soft_i2c_write_byte(reg)) 
        goto read_fail; 
 
    /* 重复启动，发送设备地址（读）并检查 ACK */ 
    soft_i2c_start(); 
    if (!soft_i2c_write_byte((addr << 1) | 0x01)) 
        goto read_fail; 
 
    /* 连续读取 len 字节 */ 
    for (i = 0; i < len; i++) 
    { 
        if (i < len - 1) 
            *(buf + i) = soft_i2c_read_byte(ACK); 
        else 
            *(buf + i) = soft_i2c_read_byte(NACK); 
    } 
 
    soft_i2c_stop(); 
    ret = 0; 
 
read_fail: 
    if (ret != 0) 
        soft_i2c_stop(); 
 
    /* 恢复中断 */ 
    EA = 1; 
 
    return ret; 
} 
 
u8 soft_iic_write(u8 addr, u8 reg, u8 *buf, u16 len) 
{ 
    u16 i; 
    u8 ret = 1; 
 
    if (!buf || len == 0) 
        return 1; 
 
    /* 整个I2C写操作进入临界区 */ 
    EA = 0; 
 
    soft_i2c_start(); 
 
    /* 发送设备地址（写） */ 
    if (!soft_i2c_write_byte(addr << 1)) 
        goto stop_fail; 
 
    /* 发送寄存器地址 */ 
    if (!soft_i2c_write_byte(reg)) 
        goto stop_fail; 
 
    /* 发送数据 */ 
    for (i = 0; i < len; i++) 
    { 
        if (!soft_i2c_write_byte(*(buf + i))) 
            goto stop_fail; 
    } 
 
    soft_i2c_stop(); 
    ret = 0; 
 
stop_fail: 
    if (ret != 0) 
        soft_i2c_stop(); 
 
    /* 恢复中断 */ 
    EA = 1; 
 
    return ret; 
}