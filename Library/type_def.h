/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __TYPE_DEF_H
#define __TYPE_DEF_H

//========================================================================
//                               类型定义(def.h)
//========================================================================

typedef bit BOOL;

typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

typedef signed char CHAR;
typedef signed int INT;
typedef signed long LONG;

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int32_t;

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

typedef signed char int8;
typedef signed int int16;
typedef signed long int32;

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

typedef signed char s8;
typedef signed int s16;
typedef signed long s32;

//===================================================

#define TRUE 1
#define FALSE 0

//===================================================

#define NULL 0

//===================================================

#define Priority_0 0 // 中断优先级为 0 级（最低级）
#define Priority_1 1 // 中断优先级为 1 级（较低级）
#define Priority_2 2 // 中断优先级为 2 级（较高级）
#define Priority_3 3 // 中断优先级为 3 级（最高级）

#define ENABLE 1
#define DISABLE 0

#define SUCCESS 0
#define FAIL -1

#endif
