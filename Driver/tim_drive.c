#include "tim_drive.h"

#include "STC32G_Timer.h"
#include "STC32G_NVIC.h"

void tim_init(TIM_Handle_t *htim)
{
    TIM_InitTypeDef TIM_InitStructure;

    if (!htim)
        return;

    TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;
    TIM_InitStructure.TIM_ClkSource = htim->clk_mode;
    TIM_InitStructure.TIM_ClkOut = DISABLE;
    TIM_InitStructure.TIM_Value = (0x10000 - htim->arr); // 0~65535
    TIM_InitStructure.TIM_PS = htim->psr;                // 0~255
    TIM_InitStructure.TIM_Run = DISABLE;
    Timer_Inilize(htim->id, &TIM_InitStructure);

    htim->ticks = 0;
}

float tim_get(TIM_Handle_t *htim)
{
    if (!htim)
        return;

    switch (htim->id)
    {
    case 0:
        htim->ticks = TH0 << 8 | TL0;
        break;
    case 1:
        htim->ticks = TH1 << 8 | TL1;
        break;
    case 2:
        htim->ticks = T2H << 8 | T2L;
        break;
    case 3:
        htim->ticks = T3H << 8 | T3L;
        break;
    case 4:
        htim->ticks = T4H << 8 | T4L;
        break;
    default:
        return;
    }

    return htim->ticks;
}

void tim_start(TIM_Handle_t *htim)
{
    if (!htim)
        return;

    switch (htim->id)
    {
    case 0:
        Timer0_Run(1);
        break;
    case 1:
        Timer1_Run(1);
        break;
    case 2:
        Timer2_Run(1);
        break;
    case 3:
        Timer3_Run(1);
        break;
    case 4:
        Timer4_Run(1);
        break;
    default:
        break;
    }
}

void tim_stop(TIM_Handle_t *htim)
{
    if (!htim)
        return;

    switch (htim->id)
    {
    case 0:
        Timer0_Stop();
        break;
    case 1:
        Timer1_Stop();
        break;
    case 2:
        Timer2_Stop();
        break;
    case 3:
        Timer3_Stop();
        break;
    case 4:
        Timer4_Stop();
        break;
    default:
        break;
    }
}

void tim_clr(TIM_Handle_t *htim)
{
    if (!htim)
        return;

    switch (htim->id)
    {
    case 0:
        TH0 = 0;
        TL0 = 0;
        break;
    case 1:
        TH1 = 0;
        TL1 = 0;
        break;
    case 2:
        T2H = 0;
        T2L = 0;
        break;
    case 3:
        T3H = 0;
        T3L = 0;
        break;
    case 4:
        T4H = 0;
        T4L = 0;
        break;
    default:
        break;
    }

    htim->ticks = 0;
}

void tim_intr(TIM_Handle_t *htim, u8 State, u8 Priority)
{
    if (!htim)
        return;

    switch (htim->id)
    {
    case 0:
        NVIC_Timer0_Init(State, Priority);
        break;
    case 1:
        NVIC_Timer1_Init(State, Priority);
        break;
    case 2:
        NVIC_Timer2_Init(State, Priority);
        break;
    case 3:
        NVIC_Timer3_Init(State, Priority);
        break;
    case 4:
        NVIC_Timer4_Init(State, Priority);
        break;
    default:
        break;
    }
}
