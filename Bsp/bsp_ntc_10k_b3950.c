#include "bsp_ntc_10k_b3950.h"

#include "STC32G_ADC.h"
#include "STC32G_GPIO.h"

#include "main.h"

#if USB_INFO
#include "stc32_stc8_usb.h"
#endif

const u16 adc_table[100] = {
    961, 999, 1038, 1077, 1117, 1158, 1199, 1241, 1284, 1327,    // 0 - 9
    1370, 1414, 1458, 1503, 1548, 1593, 1638, 1684, 1729, 1775,  // 10 - 19
    1821, 1866, 1912, 1957, 2003, 2048, 2092, 2137, 2181, 2224,  // 20 - 29
    2268, 2311, 2353, 2395, 2436, 2477, 2517, 2557, 2596, 2634,  // 30 - 39
    2672, 2709, 2745, 2781, 2816, 2850, 2884, 2916, 2949, 2980,  // 40 - 49
    3011, 3041, 3070, 3099, 3127, 3154, 3180, 3206, 3231, 3256,  // 50 - 59
    3280, 3303, 3326, 3348, 3369, 3390, 3410, 3430, 3449, 3467,  // 60 - 69
    3485, 3503, 3520, 3537, 3553, 3568, 3583, 3598, 3612, 3626,  // 70 - 79
    3639, 3652, 3665, 3677, 3689, 3700, 3712, 3722, 3733, 3743,  // 80 - 89
    3753, 3762, 3772, 3781, 3789, 3798, 3806, 3814, 3822, 3829}; //  90 - 99

#define TABLE_SIZE 100
#define TEMP_MIN 0
#define TEMP_MAX 99

void bsp_ntc_10k_b3950_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    P2_MODE_OUT_PP(GPIO_Pin_All);                                      // P2???????
    P3_MODE_OUT_PP(GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7); // P3.4 P3.5 P3.6 P3.7???????
    P4_MODE_OUT_PP(GPIO_Pin_All);                                      // P4???????
    P5_MODE_OUT_PP(GPIO_Pin_0 | GPIO_Pin_1);                           // P5.0 P5.1???????
    P1_MODE_IN_HIZ(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6);              // P1.0 P1.1 P1.6 ???????

    ADC_InitStructure.ADC_SMPduty = 31;
    ADC_InitStructure.ADC_CsSetup = 0;
    ADC_InitStructure.ADC_CsHold = 1;
    ADC_InitStructure.ADC_Speed = 2ADC_SPEED_2X16T;
    ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;
    ADC_Inilize(&ADC_InitStructure);
    ADC_PowerControl(ENABLE);
}

float bsp_ntc_10k_b3950_temperature_get(u16 adc_value)
{
    int i;
    float temp;

    if (adc_value <= adc_table[0])
    {
        return TEMP_MIN;
    }
    if (adc_value >= adc_table[TABLE_SIZE - 1])
    {
        return TEMP_MAX;
    }

    for (i = 0; i < TABLE_SIZE - 1; i++)
    {
        if (adc_value >= adc_table[i] && adc_value <= adc_table[i + 1])
        {
            break;
        }
    }

    temp = (TEMP_MIN + i) +
           (float)(adc_value - adc_table[i]) / (adc_table[i + 1] - adc_table[i]);

#if USB_INFO
#define VolADC 3.3f
    printf("adc = %u\n", adc_value);
    printf("voladc = %f\n", (float)adc_value / 4095 * VolADC);
    printf("temp = %f\n", temp);

#endif

    return temp;
}