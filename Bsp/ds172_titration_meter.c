#include "ds172_titration_meter.h"

#if DS_SENSOR == 172

#include "stc32g.h"
#include "STC32G_Delay.h"
#include "STC32G_GPIO.h"
#include "STC32G_Exti.h"
#include "STC32G_NVIC.h"
#include "STC32G_Timer.h"

sbit IR_EN = P2 ^ 0; // 被阻挡发光（Low）

static void Exti_config(void)
{
    EXTI_InitTypeDef Exti_InitStructure; // 结构定义

    Exti_InitStructure.EXTI_Mode = EXT_MODE_RiseFall; // 中断模式,   EXT_MODE_RiseFall,EXT_MODE_Fall
    Ext_Inilize(EXT_INT0, &Exti_InitStructure);       // 初始化
    NVIC_INT0_Init(ENABLE, Priority_3);               // 中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}
static void Timer_config(void) // 100us进IT
{
	AUXR &= 0xBF;			//Timer clock is 12T mode
	TMOD &= 0x0F;			//Set timer work mode
	TL1 = 0x38;				//Initial timer value
	TH1 = 0xFF;				//Initial timer value
	TF1 = 0;				//Clear TF1 flag
	TR1 = 1;				//Timer1 start run
}
void ds_init(void)
{
    IR_EN = 1;
    P2_MODE_OUT_PP(GPIO_Pin_0);
    P3_MODE_IO_PU(GPIO_Pin_2);
    P3_PULL_UP_ENABLE(GPIO_Pin_2);
    delay_ms(100);
    Exti_config();
    Timer_config();
}

void ds_update(float *dat)
{
        *dat = titra_count/2;//一次滴定会进入两次外部中断
}

#endif