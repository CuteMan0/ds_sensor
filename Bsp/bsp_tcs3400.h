#ifndef __BSP_TCS3400_H
#define __BSP_TCS3400_H

#include "type_def.h"

// TCS3400 I2C 地址（7位地址）
#define TCS3400_I2C_ADDR 0x39

// 寄存器地址（通过命令寄存器访问）
#define TCS3400_ENABLE 0x80
#define TCS3400_ATIME 0x81
#define TCS3400_WTIME 0x83
#define TCS3400_AILTL 0x84
#define TCS3400_AILTH 0x85
#define TCS3400_AIHTL 0x86
#define TCS3400_AIHTH 0x87
#define TCS3400_PERS 0x8C
#define TCS3400_CONFIG 0x8D
#define TCS3400_CONTROL 0x8F

#define TCS3400_ID 0x92
#define TCS3400_STATUS 0x93
#define TCS3400_CDATAL 0x94 // Clear 数据低字节
#define TCS3400_CDATAH 0x95 // Clear 数据高字节
#define TCS3400_RDATAL 0x96
#define TCS3400_RDATAH 0x97
#define TCS3400_GDATAL 0x98
#define TCS3400_GDATAH 0x99
#define TCS3400_BDATAL 0x9A
#define TCS3400_BDATAH 0x9B
#define TCS3400_ALCLEAR 0xE7

#define TCS3400_IR 0xC0

// ENABLE 寄存器位
#define TCS3400_ENABLE_AIEN 0x10
#define TCS3400_ENABLE_WEN 0x08
#define TCS3400_ENABLE_AEN 0x02
#define TCS3400_ENABLE_PON 0x01

typedef struct
{
    //    u16 clear;
    float red;
    float green;
    float blue;
} tcs3400_data_t;

void tcs3400_init(void);
void tcs3400_read_color(tcs3400_data_t *dev);
// void tcs3400_write_register(u8 reg, u8 val);
// void tcs3400_read_register(u8 reg, u8 *val);
// int8 tcs3400_read_data(u8 dev_addr, u8 reg_addr, u8 *dat, u16 len);

#endif