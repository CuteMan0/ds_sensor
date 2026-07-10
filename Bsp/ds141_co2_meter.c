#include "ds141_co2_meter.h"

#if DS_SENSOR == 141

#include "STC32G_GPIO.h"

#include "bsp_ntc_10k_b3950.h"

#include "adc_drive.h"

sbit hot_lamp = P1 ^ 6;
#define LAMP_ON() (hot_lamp = 1)
#define LAMP_OFF() (hot_lamp = 0)

ADC_Handle_t adc0;
ADC_Handle_t adc1;
ADC_Handle_t adc8;

void ds_init(void)
{
    P1_MODE_OUT_PP(GPIO_Pin_6); // P1.6Êä³ö
    adc_init(&adc0, 0, 3.3f);
    adc_init(&adc1, 1, 3.3f);
    adc_init(&adc8, 8, 3.3f);

    LAMP_OFF();
}

void ds_update(float *dat)
{
    static u8 tick = 0;
    static float temp,ref_channel,sig_channel;
    tick++;
    if (tick == 10)// 10*50ms
        LAMP_ON();
//    if (tick > 15)
//    {
        adc_get(&adc0);
        adc_get(&adc1);
        adc_get(&adc8);
        ref_channel = adc0.raw;
        sig_channel = adc1.raw;
        temp = adc8.raw;
        temp = bsp_ntc_10k_b3950_temperature_get(temp);
        *dat = sig_channel - ref_channel;
//    }
    if (tick == 20)
    {
        tick = 0;
        LAMP_OFF();
    }
}

#endif