#include "ds144_do_meter.h"

#if DS_SENSOR == 144

#include "STC32G_GPIO.h"
#include "STC32G_EEPROM.h"

#include "adc_drive.h"
#include "key_drive.h"
#include "led_drive.h"

#define GAIN 51.0f
#define K_cal(vol) (k_val = (8.25f / (vol - zero_point))) // 饱和氧校准,确定斜率
#define B_cal(vol) (zero_point = vol)                     // 零氧校准,确定截距

ADC_Handle_t adc0;
static volatile u8 calibration_pending = 0; // 校准标志
static volatile u8 led_flash = 0;           // LED闪烁标志

static volatile float k_val = 0.26f;
static volatile float zero_point = 0.6f; // mV

u8 flag_cal = 1; // 校准模式标志

void ds_init(void)
{
    u8 tmp[4];
    adc_init(&adc0, 0, 3.3f);

    EEPROM_read_n(0, tmp, sizeof(tmp));
    if (tmp[0] != 0xff && tmp[1] != 0xff && tmp[2] != 0xff && tmp[3] != 0xff)
    {
        k_val = (float)((u16)(tmp[0] << 8) + tmp[1]) / 1000;
        zero_point = (float)((u16)(tmp[2] << 8) + tmp[3]) / 1000;
    }
}

void ds_update(float *dat)
{
    float do_val = 0.0f;

    do_val = adc_get(&adc0) / GAIN * 1000.0f; // mV
    if (1 == flag_key)
    {
        flag_key = 0;
        flag_cal = !flag_cal; // 切换零氧校准 (0) 和 饱和校准(1)
        if (1 == flag_cal)    // 饱和校准
        {
            DIS_LED_Just_One_Enable(3);
        }
        else // 零氧校准
        {
            DIS_LED_Just_One_Enable(2);
        }
    }

    if (2 == flag_key) // 长按，开始校准
    {
        flag_key = 0;
        if (0 == flag_cal)
        {
            B_cal(do_val); // 校准截距
        }
        else
        {
            K_cal(do_val); // 校准斜率
        }
        calibration_pending = 1;
    }

    *dat = k_val * (do_val - zero_point);
}

void ProcessCalibration(void)
{
    u8 tmp[4];

    if (calibration_pending)
    {
        calibration_pending = 0;

        /* 以 u16×2 打包格式写入EEPROM，与 ds_init 读取格式一致 */
        tmp[0] = (u8)((u16)(k_val * 1000) >> 8);
        tmp[1] = (u8)((u16)(k_val * 1000) & 0xFF);
        tmp[2] = (u8)((u16)(zero_point * 1000) >> 8);
        tmp[3] = (u8)((u16)(zero_point * 1000) & 0xFF);

        EEPROM_write_n(0,
                       tmp,
                       sizeof(tmp));
        led_flash = 1;
    }
}

void Led_Task(void)
{
    static u8 cnt = 0;
    static bit on = 0;
    static u8 flash_cnt = 0;

    if (!led_flash)
        return;

    cnt++;

    if (cnt >= 10)
    {
        cnt = 0;

        on = !on;

        if (on)
        {
            if (1 == flag_cal) // 饱和校准
            {
                DIS_LED_Just_One_Enable(3);
            }
            else // 零氧校准
            {
                DIS_LED_Just_One_Enable(2);
            }
        }
        else
        {
            DIS_LED_ALL_off();
        }
    }

    if (cnt == 0)
    {
        flash_cnt++;

        if (flash_cnt >= 6)
        {
            flash_cnt = 0;
            led_flash = 0;

            if (1 == flag_cal)
            {
                DIS_LED_Just_One_Enable(3);
            }
            else
            {
                DIS_LED_Just_One_Enable(2);
            }
        }
    }
}

#endif