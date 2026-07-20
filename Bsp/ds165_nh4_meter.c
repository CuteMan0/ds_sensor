#include "ds165_nh4_meter.h"

#if DS_SENSOR == 165

#include "stc32g.h"
#include "STC32G_Delay.h"
#include "STC32G_EEPROM.h"

#include "adc_drive.h"
#include "key_drive.h"
#include "led_drive.h"

#include <math.h>

float ppm_1e2, ppm_1e3 = 0.0f;

union FloatUnion
{
    float value;
    u8 bytes[4]; // 4个字节 将float分解为字节数组
};

static float k = 0.069f; // 标定斜率
static float d = 0.08f;  // 标定截距

static u8 calibration_pending = 0; // 校准延迟处理变量
static u8 led_flash = 0;           // LED闪烁标志
static u8 flag_cal = 0;            // 校准标志：1=1000ppm校准, 0=100ppm校准

// 校准状态定义
typedef enum
{
    CAL_IDLE = 0,
    CAL_WAIT_100PPM,
    CAL_SAMPLING_100PPM,
    CAL_WAIT_1000PPM,
    CAL_SAMPLING_1000PPM,
    CAL_COMPUTE,
    CAL_DONE
} CalState_t;

// 全局变量
static CalState_t cal_state = CAL_IDLE;
static float cal_vol_100ppm = 0.0f;
static float cal_vol_1000ppm = 0.0f;
static u8 cal_sample_cnt = 0;
static float cal_vol_sum = 0.0f;

ADC_Handle_t adc0;

static void Compute_Calibration(void);
static void Key_LongPress_Handler(void);
static void Key_ShortPress_Handler(void);
static void Update_Cal_Sampling(float adc_vol);
static void EEPROM_WriteFloat(u32 addr, float value);
static float EEPROM_ReadFloat(u32 addr);

void ds_init(void)
{
    adc_init(&adc0, 0, 2.5f);

    // 读取k
    k = EEPROM_ReadFloat(0);
    // 读取d
    d = EEPROM_ReadFloat(4);

    // 检查有效性（k为NaN或超出范围则用默认值）
    if ((k != k) || (d != d) ||
        k < -1.0f || k > 1.0f)
    {
        k = 0.069f;
        d = 0.08f;
    }
}

void ds_update(float *dat)
{
    float adc_vol = 0.0f;

    adc_vol = adc_get(&adc0);

    // 处理采样状态（传入当前ADC值）
    Update_Cal_Sampling(adc_vol);

    // 处理校准计算（只在计算状态下才执行）
    if (CAL_COMPUTE == cal_state)
    {
        Compute_Calibration();
    }

    // 按键处理
    if (2 == flag_key) // 长按
    {
        flag_key = 0;
        Key_LongPress_Handler();
    }
    else if (1 == flag_key) // 短按
    {
        flag_key = 0;
        Key_ShortPress_Handler();
    }

    // 根据状态返回不同数据
    if (CAL_IDLE != cal_state)
    {
        // 校准模式下返回实时 ADC 电压值
        *dat = adc_vol * 1000; // mV
    }
    else
    {
        // 正常模式下计算 ppm 值
        if (k != 0.0f) // 避免除零
        {
            *dat = pow(10.0f, (adc_vol - d) / k);
        }
        else
        {
            *dat = 0.0f;
        }
    }
}

void Led_Task(void)
{
    static u8 cnt = 0;
    static bit on = 0;
    static u8 flash_cnt = 0;

    // 日常运行状态：3号灯常亮
    if (CAL_IDLE == cal_state && !led_flash)
    {
        DIS_LED_Just_One_Enable(3);
        return;
    }

    if (!led_flash)
        return;

    cnt++;

    if (cnt >= 10)
    {
        cnt = 0;

        on = !on;

        if (on)
        {
            if (1 == flag_cal) // 1000ppm校准
            {
                DIS_LED_Just_One_Enable(3);
            }
            else // 100ppm校准
            {
                DIS_LED_Just_One_Enable(2);
            }
        }
        else
        {
            DIS_LED_ALL_off();

            // LED熄灭时计数闪烁次数
            flash_cnt++;

            // 校准等待状态持续闪烁，不计数
            if (CAL_WAIT_100PPM == cal_state || CAL_WAIT_1000PPM == cal_state)
            {
                // 等待状态：不限制闪烁次数，重置计数器
                flash_cnt = 0;
            }
            else if (flash_cnt >= 6)
            {
                // 闪烁6次后停止
                flash_cnt = 0;
                led_flash = 0;

                // 校准完成，回到空闲状态
                if (CAL_DONE == cal_state)
                {
                    cal_state = CAL_IDLE;
                }

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
}

void ProcessCalibration(void)
{
    u8 tmp[8] = {0};
    u8 i;

    union FloatUnion u_k, u_d;

    if (calibration_pending)
    {
        calibration_pending = 0;

        EEPROM_WriteFloat(0, k);
        EEPROM_WriteFloat(4, d);

        // 校准完成提示：3号灯闪烁
        flag_cal = 1;
        led_flash = 1;
    }
}

// 校准采样更新函数
static void Update_Cal_Sampling(float adc_vol)
{
    // 只在采样状态下执行采样
    if (CAL_SAMPLING_100PPM != cal_state &&
        CAL_SAMPLING_1000PPM != cal_state)
        return;

    // 100ppm采样状态
    if (CAL_SAMPLING_100PPM == cal_state)
    {
        if (cal_sample_cnt < 20) // 采样20次
        {
            cal_vol_sum += adc_vol; // 使用传入的ADC值
            cal_sample_cnt++;
        }

        // 采样完成
        if (cal_sample_cnt >= 20)
        {
            cal_vol_100ppm = cal_vol_sum / 20.0f;
            cal_state = CAL_WAIT_1000PPM;
            cal_sample_cnt = 0;
            cal_vol_sum = 0.0f;

            // 提示切换到1000ppm：3号灯闪烁
            flag_cal = 1;
            led_flash = 1;
        }
    }
    // 1000ppm采样状态
    else if (CAL_SAMPLING_1000PPM == cal_state)
    {
        if (cal_sample_cnt < 20) // 采样20次
        {
            cal_vol_sum += adc_vol; // 使用传入的ADC值
            cal_sample_cnt++;
        }

        // 采样完成
        if (cal_sample_cnt >= 20)
        {
            cal_vol_1000ppm = cal_vol_sum / 20.0f;
            cal_state = CAL_COMPUTE;
        }
    }
}

// 长按按键处理
static void Key_LongPress_Handler(void)
{
    if (CAL_IDLE == cal_state)
    {
        // 进入校准模式：2号灯闪烁，提示100ppm校准等待
        cal_state = CAL_WAIT_100PPM;
        cal_sample_cnt = 0;
        cal_vol_sum = 0.0f;

        flag_cal = 0;  // 100ppm校准
        led_flash = 1; // 2号灯闪烁
    }
    else if (CAL_WAIT_100PPM == cal_state || CAL_WAIT_1000PPM == cal_state)
    {
        // 在校准等待状态长按取消校准
        cal_state = CAL_IDLE;
        cal_sample_cnt = 0;
        cal_vol_sum = 0.0f;
        led_flash = 0; // 停止闪烁

        DIS_LED_Just_One_Enable(3); // 恢复3号灯常亮
    }
}

// 短按按键处理
static void Key_ShortPress_Handler(void)
{
    if (CAL_WAIT_100PPM == cal_state)
    {
        // 第一次短按：开始100ppm采样，2号灯常亮
        cal_state = CAL_SAMPLING_100PPM;
        cal_sample_cnt = 0;
        cal_vol_sum = 0.0f;

        led_flash = 0;              // 停止闪烁
        DIS_LED_Just_One_Enable(2); // 2号灯常亮表示采样中
    }
    else if (CAL_WAIT_1000PPM == cal_state)
    {
        // 第二次短按：开始1000ppm采样，3号灯常亮
        cal_state = CAL_SAMPLING_1000PPM;
        cal_sample_cnt = 0;
        cal_vol_sum = 0.0f;

        led_flash = 0;              // 停止闪烁
        DIS_LED_Just_One_Enable(3); // 3号灯常亮表示采样中
    }
}

// 计算斜率k和截距d
static void Compute_Calibration(void)
{
    float log_100 = log10(100.0f);   // = 2.0
    float log_1000 = log10(1000.0f); // = 3.0

    if (cal_state != CAL_COMPUTE)
        return;

    k = (cal_vol_1000ppm - cal_vol_100ppm) / (log_1000 - log_100);
    d = cal_vol_100ppm - k * log_100;

    // 触发EEPROM存储
    calibration_pending = 1;
    cal_state = CAL_DONE;
}

static void EEPROM_WriteFloat(u32 addr, float value)
{
    union FloatUnion u;
    u.value = value;
    EEPROM_write_n(addr, u.bytes, 4);
}

static float EEPROM_ReadFloat(u32 addr)
{
    union FloatUnion u;
    EEPROM_read_n(addr, u.bytes, 4);
    return u.value;
}

#endif