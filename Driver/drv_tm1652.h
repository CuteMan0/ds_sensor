#ifndef __DRV_TM1652_H__
#define __DRV_TM1652_H__

#include "type_def.h"

#define TM1652_DIS          0x18

#define TM1652_GR1_ADDR     0x08
#define TM1652_GR2_ADDR     0x88
#define TM1652_GR3_ADDR     0x48
#define TM1652_GR4_ADDR     0xC8
#define TM1652_GR5_ADDR     0x28

#define TM1652_SEG_BLANK    0x00
#define TM1652_SEG_MINUS    0x40

typedef void (*tm1652_write_byte_t)(u8 dat);

/*
 * TM1652设备对象
 */
typedef struct
{
    tm1652_write_byte_t write_byte;
} tm1652_t;


/* 初始化 */
void tm1652_init(tm1652_t *dev, tm1652_write_byte_t write_byte);

/* 发送数据 */
void tm1652_send(tm1652_t *dev, u8 *buf, u8 len);

/* 关闭显示 */
void tm1652_off(tm1652_t *dev);

/* 清屏 */
void tm1652_clear(tm1652_t *dev);

/* 设置单个数码管数据 */
void tm1652_set_digit(tm1652_t *dev, u8 addr, u8 seg);

/* 显示整数 */
void tm1652_show_int(tm1652_t *dev, int value);

/* 浮点数转换成段码 */
void tm1652_float_to_segment(float value,
                             u8 *buf,
                             u8 digit_count,
                             u8 max_decimals);

/* 显示浮点数 */
void tm1652_show_float(tm1652_t *dev,
                       float value,
                       u8 digit_count,
                       u8 max_decimals);

#endif