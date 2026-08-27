#ifndef __KEY_drive_H
#define __KEY_drive_H

#include "stc32g.h"

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

//========================================================================
//                              脚位定义
//========================================================================
sbit HOME_key = P3 ^ 2; // home按键
extern u8 flag_key;

//========================================================================
//                              本地函数声明
//========================================================================
extern void KEY_GPIO_init(void);
extern void KEY_Test(void);
void Scan_Key(void);

#endif
