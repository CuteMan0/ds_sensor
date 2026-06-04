#ifndef __LEDdrive_H
#define __LEDdrive_H

#include "stc32g.h"

//========================================================================
//                              LED灯号
//========================================================================
#define LED_Num1 1 // LED灯1#
#define LED_Num2 2 // LED灯2#
#define LED_Num3 3 // LED灯3#

#define Range1_LED_en 1 // 量程1 LED指示灯
#define Range2_LED_en 2 // 量程2 LED指示灯
#define Range3_LED_en 3 // 量程3 LED指示灯
//========================================================================
//                              LED灯端口定义
//========================================================================
sbit Range1_LED = P2 ^ 1; // D1
sbit Range2_LED = P2 ^ 2; // D2
sbit Range3_LED = P2 ^ 3; // D3
sbit BELL = P3 ^ 6;       // bell

//========================================================================
//                              本地函数声明
//========================================================================
extern void DIS_LED_GPIO_config(void);
extern void DIS_LED_init(void);
extern void DIS_LED_cal(void);
extern void DIS_LED_ALL_on(void);
extern void DIS_LED_ALL_off(void);
extern void DIS_LED_Enable(unsigned char LED_num);
extern void DIS_LED_Disable(unsigned char LED_num);
extern void DIS_LED_Just_One_Enable(unsigned char LED_num);
extern void DIS_LED_Test(void);
extern void LED_flashing_slowly(unsigned char LED_number, unsigned char Times);
extern void LED_flashing_rapidly(unsigned char LED_number, unsigned char Times);
#endif
