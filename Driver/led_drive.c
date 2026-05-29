#include "led_drive.h"

#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

//========================================================================
// 函数: void RGB_LCD_GPIO_config(void)
// 描述: LED灯驱动IO口配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void DIS_LED_GPIO_config(void)
{
    P2_MODE_OUT_PP(GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); // P2.1 P2.2 P2.3设置为推挽输出

    P3_MODE_OUT_PP(GPIO_Pin_6); // P0.3 P0.4设置成推挽输出
}

//========================================================================
// 函数: void DIS_LED_init(void)
// 描述: LED灯初始化.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void DIS_LED_init(void)
{
    DIS_LED_GPIO_config();
    Range1_LED = 0;
    Range2_LED = 0;
    Range3_LED = 1;//初始化点亮蓝灯
}

void DIS_LED_cal(void)
{
    Range1_LED = 0;
    Range2_LED = 1;//校准点亮绿灯
    Range3_LED = 0;
}

//========================================================================
// 函数: void DIS_LED_ALL_on(void)
// 描述: LED灯全亮.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void DIS_LED_ALL_on(void)
{
    Range1_LED = 1; // 点亮Range1_LED
    Range2_LED = 1; // 点亮Range2_LED
    Range3_LED = 1; // 点亮Range3_LED
}

//========================================================================
// 函数: void DIS_LED_ALL_off(void)
// 描述: LED灯全灭.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void DIS_LED_ALL_off(void)
{
    Range1_LED = 0; // 熄灭Range1_LED
    Range2_LED = 0; // 熄灭Range2_LED
    Range3_LED = 0; // 熄熄灭Range3_LED
}

//========================================================================
// 函数: void DIS_LED_Enable(unsigned char LED_num)
// 描述: 点亮任意LED.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void DIS_LED_Enable(unsigned char LED_num)
{
    switch (LED_num)
    {
    case 1:
        Range1_LED = 1;
        break; // 点亮Range1_LED;
    case 2:
        Range2_LED = 1;
        break; // 点亮Range2_LED;
    case 3:
        Range3_LED = 1;
        break; // 点亮Range3_LED;
        //				default:	;
    }
}

//========================================================================
// 函数: void DIS_LED_Disable(unsigned char LED_num)
// 描述: 熄灭任意LED.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void DIS_LED_Disable(unsigned char LED_num)
{
    switch (LED_num)
    {
    case 1:
        Range1_LED = 0;
        break; // 熄灭Range1_LED ;
    case 2:
        Range2_LED = 0;
        break; // 熄灭Range2_LED ;
    case 3:
        Range3_LED = 0;
        break; // 熄熄灭Range3_LED ;
        //				default:	;
    }
}

//========================================================================
// 函数: void DIS_LED_Just_One_Enable(void)
// 描述: 仅点亮一个LED.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void DIS_LED_Just_One_Enable(unsigned char LED_num)
{
    DIS_LED_ALL_off(); // 熄灭所有DIS_LED;
    switch (LED_num)
    {
    case 0:
        DIS_LED_ALL_off();
        break; // 熄灭所有DIS_LED;
    case 1:
        Range1_LED = 1;
        break; // 点亮Range1_LED;
    case 2:
        Range2_LED = 1;
        break; // 点亮Range2_LED;
    case 3:
        Range3_LED = 1;
        break; // 点亮Range3_LED;
        //				default:	;
    }
}

//========================================================================
// 函数: void DIS_LED_Test(void)
// 描述: LED灯初测试.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void DIS_LED_Test(void)
{
    DIS_LED_ALL_off();
    delay_ms(1000);

    DIS_LED_ALL_on();
    delay_ms(1000);
}

//========================================================================
// 函数: void LED_flashing_slowly(unsigned char LED_number,unsigned char Times)
// 描述: LED缓慢闪烁.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2025-04-16
void LED_flashing_slowly(unsigned char LED_number, unsigned char Times)
{
    unsigned char pos;
    for (pos = 0; pos < Times; pos++)
    {
        DIS_LED_Enable(LED_number);
        delay_ms(500);
        DIS_LED_Disable(LED_number);
        delay_ms(500);
    }
}

//========================================================================
// 函数: void LED_flashing_rapidly(unsigned char LED_number,unsigned char Times)
// 描述: LED快速闪烁.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2025-04-16
void LED_flashing_rapidly(unsigned char LED_number, unsigned char Times)
{
    unsigned char pos;
    for (pos = 0; pos < Times; pos++)
    {
        DIS_LED_Enable(LED_number);
        delay_ms(200);
        DIS_LED_Disable(LED_number);
        delay_ms(200);
    }
}
