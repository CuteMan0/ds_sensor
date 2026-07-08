// 测试工作频率为 24 MHz
#include "stc32g.h"

#include <stdio.h>
#include <math.h>

#include "ADC_drive.h"
#include "LED_drive.h"
#include "USB_CDC_drive.h"

#include "fft.h"
#include "log.h"
#include "task_scheduler.h"

#define PI 3.1415926f
#define FS 12.5f
#define N MAX_FFT_N
float xdata adc_buf[MAX_FFT_N];
complex_t xdata fft_out[MAX_FFT_N];
float xdata mag[MAX_FFT_N];

u8 data_ready;

ADC_Handle_t adc0;

float FFT_FreDetect(float *raw, float sample_rate)
{
#define THRESHOLD 0.08f
    u16 i, max_bin;
    float dc, freq, max_val, min_freq, max_freq, f;
    float tmp[MAX_FFT_N];
    
    /* BPF */
    min_freq = 0.05f;
    max_freq = 1.5f;

    //去直流
    dc = 0;
    for (i = 0; i < MAX_FFT_N; i++)
        dc += raw[i];
    dc /= (float)MAX_FFT_N;
    
    for (i = 0; i < MAX_FFT_N; i++)
        tmp[i] = raw[i] - dc;

    /* Hann */
    for (i = 0; i < MAX_FFT_N; i++)
        tmp[i] = tmp[i] * (0.5f - 0.5f * cos(2 * PI * i / (MAX_FFT_N - 1)));

    /* FFT */
    fft_run(tmp, fft_out);
    fft_abs(fft_out, mag);

    /* 频率峰值 */
    max_val = 0;
    max_bin = 0;
    for (i = 1; i < MAX_FFT_N / 2; i++)
    {
        f = (float)i * sample_rate / (float)MAX_FFT_N;
        if (f >= min_freq && f <= max_freq)
        {
            if (mag[i] > max_val)
            {
                max_val = mag[i];
                max_bin = i;
            }
        }
    }

    if (0 == max_bin || THRESHOLD > max_val)
        return 0.0f;

    freq = (float)max_bin * sample_rate / (float)MAX_FFT_N;
    return freq;
}

void task_sensor(void)
{
    float breath_hz;
    float breath_bpm;
    if(!data_ready)
        return;

    data_ready = 0;

    breath_hz  = FFT_FreDetect(adc_buf, FS);
    breath_bpm = breath_hz * 60.0f;

    printf("Breath: %.1f BPM\r\n",breath_bpm);

}

void task_update(void)
{
    static u16 fill = 0;
    u16 k;

    adc_get(&adc0); 

    //填充FFT数组
    if (fill < N)
    {
        adc_buf[fill] = adc0.voltage;
        fill++;

        if (N == fill)
            data_ready = 1;
    }
    else{
        for (k = 0; k < N - 1; k++)
            adc_buf[k] = adc_buf[k + 1];
        adc_buf[N - 1] = adc0.voltage;
        
        data_ready = 1;
    }
}

void main(void)
{
    WTST = 0;
    EAXFR = 1;
    CKCON = 0;

    USB_CDC_Initialization();
    EA = 1;
    while (DeviceState != DEVSTATE_CONFIGURED);
    
    led_init();
    adc_init(&adc0, 0, 3.3f);
    fft_init();
    
    task_scheduler_init();
    task_register(task_update, 1000/FS, 1);
    task_register(task_sensor, 1000, 1);
    
    while (1)
    {
        task_scheduler_run();
    }
}
