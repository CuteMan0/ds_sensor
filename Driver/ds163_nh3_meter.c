#include "ds163_nh3_meter.h"
// MP-702
#if DS_SENSOR == 163

#include "adc_drive.h"
#include "led_drive.h"

#include "lut.h"

#define V0 1000

ADC_Handle_t adc0;

const LUT_2D mp702_lut[] =
    {
        {150, 0},
        {600, 5},
        {950, 10},
        {1300, 15},
        {1600, 20},
        {1900, 30},
        {2100, 40},
        {2200, 50},
        {2350, 75},
        {2450, 100}};

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

void ds_update(void)
{
    u16 nh3_val, adc_vol_mv = 0.0f;

    adc_vol_mv = (u16)(adc_get(&adc0) * 2000);

    nh3_val = LUT_BinaryInterp_u16(mp702_lut,
                                   sizeof(mp702_lut) / sizeof(mp702_lut[0]),
                                   adc_vol_mv - V0);

    dat_for_printf = nh3_val;
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}

void ds_printf(void)
{
    printf_usb("NH3:%.2fppm\r\n", dat_for_printf);
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif
