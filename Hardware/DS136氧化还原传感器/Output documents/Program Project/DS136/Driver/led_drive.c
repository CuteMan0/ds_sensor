#include "led_drive.h"
#include "STC32G_GPIO.h"

//------------------------------------------------------------
// LED 初始化
//------------------------------------------------------------
void led_init(void)
{
    P2_MODE_OUT_PP(GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);

    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
}

static void led_all_on(void)
{
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
}

static void led_all_off(void)
{
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
}

//------------------------------------------------------------
// 点亮指定 LED
//------------------------------------------------------------
void led_enable(u8 led)
{
    switch (led)
    {
        case 1: LED1 = 1; break;
        case 2: LED2 = 1; break;
        case 3: LED3 = 1; break;
        default: break;
    }
}

//------------------------------------------------------------
// 熄灭指定 LED
//------------------------------------------------------------
void led_disable(u8 led)
{
    switch (led)
    {
        case 1: LED1 = 0; break;
        case 2: LED2 = 0; break;
        case 3: LED3 = 0; break;
        default: break;
    }
}

//------------------------------------------------------------
// 仅点亮一个 LED
//------------------------------------------------------------
void led_just_one_enable(u8 led)
{
    led_all_off();

    switch (led)
    {
        case 1: LED1 = 1; break;
        case 2: LED2 = 1; break;
        case 3: LED3 = 1; break;
        default: break;
    }
}

//------------------------------------------------------------
// LED 测试
//------------------------------------------------------------
void led_test(u8 step)
{
    if (step == 0)
        led_all_off();
    else if (step == 1)
        led_all_on();
}

//------------------------------------------------------------
// LED 闪烁
//------------------------------------------------------------
void led_flashing(u8 led)
{
    static u8 state = 1;
    if (state%2)
        led_enable(led);
    else
        led_disable(led);
    state++; 
}
