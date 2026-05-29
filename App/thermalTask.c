#include "thermalTask.h"
//------------------------------STC lib--------------------------
#include "STC32G_ADC.h"
#include "STC32G_Delay.h"
//------------------------------bsp lib------------------------------
#include "bsp_ntc_10k_b3950.h"

sbit LED_W1 = P5 ^ 0;
sbit LED_W2 = P3 ^ 4;
sbit LED_W3 = P5 ^ 1;
sbit LED_W4 = P3 ^ 5;
sbit LED_W5 = P3 ^ 6;

sbit LED_R1 = P4 ^ 6;
sbit LED_R2 = P4 ^ 5;
sbit LED_R3 = P2 ^ 7;
sbit LED_R4 = P2 ^ 6;
sbit LED_R5 = P2 ^ 5;
sbit LED_R6 = P2 ^ 4;
sbit LED_R7 = P2 ^ 3;
sbit LED_R8 = P2 ^ 2;
sbit LED_R9 = P2 ^ 1;
sbit LED_R10 = P2 ^ 0;
sbit LED_R11 = P4 ^ 4;
sbit LED_R12 = P4 ^ 3;
sbit LED_R13 = P4 ^ 2;
sbit LED_R14 = P4 ^ 1;
sbit LED_R15 = P3 ^ 7;

u8 channel_temp[5] = {0};
u32 loop_counter = 0;

void led_check(void)
{
    LED_W1 = LED_W2 = LED_W3 = LED_W4 = LED_W5 = 0;
    LED_R1 = LED_R2 = LED_R3 = LED_R4 = LED_R5 =
        LED_R6 = LED_R7 = LED_R8 = LED_R9 = LED_R10 =
            LED_R11 = LED_R12 = LED_R13 = LED_R14 = LED_R15 = 0;
    delay_ms(50);
}

void sample_all_channels(void)
{
    channel_temp[0] = (u8)bsp_ntc_10k_b3950_temperature_get(Get_ADCResult(1));
    // channel_temp[1] = (u8)bsp_ntc_10k_b3950_temperature_get(Get_ADCResult(6));
    // channel_temp[2] = (u8)bsp_ntc_10k_b3950_temperature_get(Get_ADCResult(7));
    // channel_temp[3] = (u8)bsp_ntc_10k_b3950_temperature_get(Get_ADCResult(8));
    // channel_temp[4] = (u8)bsp_ntc_10k_b3950_temperature_get(Get_ADCResult(9));
}

void update_led_display(u8 temperature)
{
    LED_R1 = LED_R2 = LED_R3 = LED_R4 = LED_R5 =
        LED_R6 = LED_R7 = LED_R8 = LED_R9 = LED_R10 =
            LED_R11 = LED_R12 = LED_R13 = LED_R14 = LED_R15 = 1;

    if (temperature > 0)
        LED_R15 = 0;
    if (temperature > 5)
        LED_R14 = 0;
    if (temperature > 10)
        LED_R13 = 0;
    if (temperature > 15)
        LED_R12 = 0;
    if (temperature > 20)
        LED_R11 = 0;
    if (temperature > 25)
        LED_R10 = 0;
    if (temperature > 30)
        LED_R9 = 0;
    if (temperature > 35)
        LED_R8 = 0;
    if (temperature > 40)
        LED_R7 = 0;
    if (temperature > 45)
        LED_R6 = 0;
    if (temperature > 50)
        LED_R5 = 0;
    if (temperature > 55)
        LED_R4 = 0;
    if (temperature > 60)
        LED_R3 = 0;
    if (temperature > 65)
        LED_R2 = 0;
    if (temperature > 70)
        LED_R1 = 0;
}

void display_time_divider(void)
{
    static u8 current_channel = 0;

    LED_W1 = 1;
    LED_W2 = 1;
    LED_W3 = 1;
    LED_W4 = 1;
    LED_W5 = 1;

    switch (current_channel)
    {
    case 0:
        LED_W1 = 0;
        break;
    case 1:
        LED_W2 = 0;
        break;
    case 2:
        LED_W3 = 0;
        break;
    case 3:
        LED_W4 = 0;
        break;
    case 4:
        LED_W5 = 0;
        break;
    }

    update_led_display(channel_temp[current_channel]);

    current_channel++;
    if (current_channel >= 5)
        current_channel = 0;
}