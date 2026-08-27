#include "ds158_ecg_meter.h"

#if DS_SENSOR == 158

#include "adc_drive.h"

/* 采样与陷波参数 */
#define ECG_FS 1000.0f /* 采样频率 1kHz */
#define ECG_F0 50.0f   /* 50Hz 工频陷波 */
#define ECG_R 0.9f     /* 极半径 */

static ADC_Handle_t adc0;

/* 三级 50Hz 陷波器，增强工频抑制 */
static NotchFilterFixed filter1;
static NotchFilterFixed filter2;
static NotchFilterFixed filter3;

/* 归一化后的输出 */
static volatile float g_ecg_normalized;
static volatile float g_ecg_mv;
void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);

    notch_fixed_init(&filter1, ECG_FS, ECG_F0, ECG_R);
    notch_fixed_init(&filter2, ECG_FS, ECG_F0, ECG_R);
    notch_fixed_init(&filter3, ECG_FS, ECG_F0, ECG_R);
}

void ds_update(void)
{
    q15_t input;
    q15_t output;

    adc_get(&adc0);

    input = adc_to_q15(adc0.raw);

    output = notch_fixed_process(&filter1, input);
    output = notch_fixed_process(&filter2, output);
    output = notch_fixed_process(&filter3, output);

    /*
     * 归一化ECG
     */
    g_ecg_normalized = (float)output / 32768.0f;

    /*
     * 换算为人体ECG电压
     *
     * ADC满量程：3.3V
     * ADC中点：约1.65V
     * AD8232总增益：约1100
     */
    g_ecg_mv = g_ecg_normalized * 1.5f;

    dat_for_printf = g_ecg_mv;

    task_delay_ms(1);
}

void ds_printf(void)
{
    printf_usb("%.4f\r\n", g_ecg_mv); // mV
}

void ds_calib(void)
{
    /* 无校准需求 */
}

#endif