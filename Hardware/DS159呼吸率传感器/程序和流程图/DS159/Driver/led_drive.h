#ifndef __led_DRIVE_H__
#define __led_DRIVE_H__

#include "stc32g.h"
#include "type_def.h"

sbit LED1 = P2^3;	//LED1
sbit LED2 = P2^2;	//LED2
sbit LED3 = P2^1;	//LED3

//-----------------------------------------------
// LED 驱动接口
//-----------------------------------------------
void led_init(void);
void led_enable(u8 led);
void led_disable(u8 led);
void led_just_one_enable(u8 led);

// 非阻塞功能
void led_test(u8 step);
void led_flashing(u8 led);

#endif 






