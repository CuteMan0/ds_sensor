#include "drv_tm1652.h"

#include <stdio.h>
#include <string.h>

static const u8 seg_table[] =
{
    0x3F,   /* 0 */
    0x06,   /* 1 */
    0x5B,   /* 2 */
    0x4F,   /* 3 */
    0x66,   /* 4 */
    0x6D,   /* 5 */
    0x7D,   /* 6 */
    0x07,   /* 7 */
    0x7F,   /* 8 */
    0x6F,   /* 9 */
    0x77,   /* A */
    0x7C,   /* b */
    0x39,   /* C */
    0x5E,   /* d */
    0x79,   /* E */
    0x71    /* F */
};


/* 奇校验 */
static u8 tm1652_get_odd_parity(u8 dat)
{
    u8 cnt;
    u8 tmp;

    cnt = 0;
    tmp = dat;

    while (tmp)
    {
        cnt ^= (tmp & 0x01);
        tmp >>= 1;
    }

    return (cnt == 0) ? 1 : 0;
}


/*
 * 初始化
 */
void tm1652_init(tm1652_t *dev, tm1652_write_byte_t write_byte)
{
    if (dev == 0)
    {
        return;
    }

    dev->write_byte = write_byte;
}


/*
 * 发送一个字节
 */
static void tm1652_write(tm1652_t *dev, u8 dat)
{
    if (dev == 0)
    {
        return;
    }

    if (dev->write_byte == 0)
    {
        return;
    }

    /*
     * TM1652要求发送奇校验
     *
     * 奇校验位由底层UART硬件发送。
     *
     * 因此这里不能直接操作S2TB8/S4TB8，
     * 而是通过底层write_byte完成。
     */
    dev->write_byte(dat);
}


/*
 * 发送数据
 */
void tm1652_send(tm1652_t *dev, u8 *buf, u8 len)
{
    u8 i;

    if (dev == 0)
    {
        return;
    }

    if (buf == 0)
    {
        return;
    }

    for (i = 0; i < len; i++)
    {
        tm1652_write(dev, buf[i]);
    }
}


/*
 * 关闭显示
 */
void tm1652_off(tm1652_t *dev)
{
    u8 buf[2];

    buf[0] = TM1652_DIS;
    buf[1] = 0x00;

    tm1652_send(dev, buf, 2);
}


/*
 * 清屏
 */
void tm1652_clear(tm1652_t *dev)
{
    u8 buf[6];

    buf[0] = TM1652_GR1_ADDR;
    buf[1] = TM1652_SEG_BLANK;
    buf[2] = TM1652_SEG_BLANK;
    buf[3] = TM1652_SEG_BLANK;
    buf[4] = TM1652_SEG_BLANK;
    buf[5] = TM1652_SEG_BLANK;

    tm1652_send(dev, buf, 6);
}


/*
 * 设置一个显示位置
 */
void tm1652_set_digit(tm1652_t *dev, u8 addr, u8 seg)
{
    u8 buf[2];

    buf[0] = addr;
    buf[1] = seg;

    tm1652_send(dev, buf, 2);
}


/*
 * 浮点数转换成段码
 *
 * 例如：
 *
 * value = 12.34
 * digit_count = 4
 * max_decimals = 2
 *
 * 得到：
 *
 * 0x06
 * 0x5B
 * 0x4F | DP
 * 0x66
 */
void tm1652_float_to_segment(float value,
                             u8 *buf,
                             u8 digit_count,
                             u8 max_decimals)
{
    char str[32];
    char fmt[8];

    u8 i;
    u8 pos;

    if (buf == 0)
    {
        return;
    }

    /*
     * 先清空
     */
    for (i = 0; i < digit_count; i++)
    {
        buf[i] = TM1652_SEG_BLANK;
    }

    /*
     * 生成格式字符串
     *
     * 例如：
     * max_decimals = 2
     *
     * fmt = "%.2f"
     */
    sprintf(fmt, "%%.%df", max_decimals);

    sprintf(str, fmt, value);

    i = 0;
    pos = 0;

    while ((str[pos] != '\0') && (i < digit_count))
    {
        if (str[pos] == '-')
        {
            buf[i] = TM1652_SEG_MINUS;
            i++;
        }
        else if (str[pos] == '.')
        {
            if (i > 0)
            {
                buf[i - 1] |= 0x80;
            }
        }
        else if ((str[pos] >= '0') && (str[pos] <= '9'))
        {
            buf[i] = seg_table[str[pos] - '0'];
            i++;
        }

        pos++;
    }
}


/*
 * 显示浮点数
 */
void tm1652_show_float(tm1652_t *dev,
                       float value,
                       u8 digit_count,
                       u8 max_decimals)
{
    u8 buf[8];
    u8 i;

    if (digit_count > 8)
    {
        digit_count = 8;
    }

    tm1652_float_to_segment(value,
                            buf,
                            digit_count,
                            max_decimals);

    /*
     * GR1开始连续写
     */
    tm1652_write(dev, TM1652_GR1_ADDR);

    for (i = 0; i < digit_count; i++)
    {
        tm1652_write(dev, buf[i]);
    }
}


/*
 * 显示整数
 */
void tm1652_show_int(tm1652_t *dev, int value)
{
    char str[12];
    u8 buf[8];
    u8 i;
    u8 pos;
    u8 len;

    sprintf(str, "%d", value);

    for (i = 0; i < 8; i++)
    {
        buf[i] = TM1652_SEG_BLANK;
    }

    len = strlen(str);

    if (len > 8)
    {
        len = 8;
    }

    i = 0;
    pos = 0;

    while ((str[pos] != '\0') && (i < len))
    {
        if (str[pos] == '-')
        {
            buf[i] = TM1652_SEG_MINUS;
        }
        else if ((str[pos] >= '0') && (str[pos] <= '9'))
        {
            buf[i] = seg_table[str[pos] - '0'];
        }

        i++;
        pos++;
    }

    tm1652_write(dev, TM1652_GR1_ADDR);

    for (i = 0; i < len; i++)
    {
        tm1652_write(dev, buf[i]);
    }
}