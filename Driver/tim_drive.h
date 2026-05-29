#ifndef __TIM_DRIVE_H
#define __TIM_DRIVE_H

#include "type_def.h"

typedef struct
{
    u8 id;
    u8 clk_mode; // 0--12T, 1--1T, 2--Ext
    u8 psr;
    u16 arr;
    u16 ticks;
} TIM_Handle_t;

void tim_start(TIM_Handle_t *htim);
void tim_stop(TIM_Handle_t *htim);
void tim_clr(TIM_Handle_t *htim);
void tim_intr(TIM_Handle_t *htim, u8 State, u8 Priority);

void tim_init(TIM_Handle_t *htim);
float tim_get(TIM_Handle_t *htim);

#endif