/*气体酒精传感器*/
#include "ds138_airC2H6O_meter.h"

#if DS_SENSOR == 138

#include "STC32G_Delay.h"
#include "STC32G_EEPROM.h"

#include "adc_drive.h"
#include "key_drive.h"
#include "led_drive.h"

#define TABLE_SIZE (sizeof(voltage_table) / sizeof(voltage_table[0]))

// 电压与浓度对照表（按电压升序排列）
static float voltage_table[] = {
    0.24f, 0.53f, 0.70f, 0.78f, 0.85f, 0.91f, 0.95f, 0.98f, 1.00f, 1.03f,
    1.05f, 1.08f, 1.12f, 1.15f, 1.18f, 1.20f, 1.23f, 1.30f, 1.33f, 1.35f,
    1.40f, 1.45f};

static const float concentration_table[] = {
    0.0f, 5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f,
    50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 120.0f, 150.0f, 160.0f, 200.0f,
    250.0f, 300.0f};

static float zero_voltage_calibrated = 0.24f; // 校准后的零点电压
ADC_Handle_t adc0;

// 校准延迟处理变量
static volatile u8 calibration_pending = 0;
static volatile float pending_zero_value = 0;

void airC2H6O_Init(void)
{
    u8 tmp[2];
    adc_init(&adc0, 0, 3.3f);

    // 从EEPROM读取校准值
    EEPROM_read_n(0, tmp, sizeof(tmp));

    if (tmp[0] != 0xff && tmp[1] != 0xff)
    {
        pending_zero_value = (float)((u16)(tmp[0] << 8) + tmp[1]) / 1000;
        zero_voltage_calibrated = pending_zero_value + 0.01f;
    }
}
void ProcessCalibration(void)
{
    u8 tmp[2];
    if (calibration_pending)
    {
        calibration_pending = 0;

        // 写入EEPROM（这里过于耗时，在低频任务中执行）
        tmp[0] = (u16)(pending_zero_value * 1000) >> 8;
        tmp[1] = (u16)(pending_zero_value * 1000);
        EEPROM_write_n(0, tmp, sizeof(tmp));
    }
}

u8 airC2H6O_Read(float *val)
{
    float voltage, new_zero;
    float zero_voltage = zero_voltage_calibrated;
    u8 i;

    // 处理短按校准请求（通知校准函数）
    if (1 == flag_key)
    {
        flag_key = 0;

        // 读取当前电压作为新的零点
        new_zero = adc_get(&adc0);

        // 检查电压是否合理（正常零点应该在0.2V-0.3V之间）
        if (new_zero >= 0.1f && new_zero <= 0.5f)
        {
            zero_voltage_calibrated = new_zero + 0.01f;

            // 标记需要保存到EEPROM
            pending_zero_value = new_zero;
            calibration_pending = 1;

            // LED指示
            DIS_LED_Just_One_Enable(2);
        }
    }
    else if (2 == flag_key) // 长按：恢复出厂设置
    {
        flag_key = 0;

        // 恢复出厂零点
        zero_voltage_calibrated = 0.24f;
        pending_zero_value = 0.24f;
        calibration_pending = 1;

        // LED指示恢复
        delay_ms(100);
        DIS_LED_Just_One_Enable(3);
    }

    voltage = adc_get(&adc0);

    // 边界检查：低于校准后的零点
    if (voltage <= zero_voltage)
    {
        *val = concentration_table[0];
        return 1;
    }

    // 边界检查：高于最大电压
    if (voltage >= voltage_table[TABLE_SIZE - 1])
    {
        *val = concentration_table[TABLE_SIZE - 1];
        return 2;
    }

    // 特殊处理：电压在零点到第一个电压点之间
    if (voltage < voltage_table[0])
    {
        // 零点到第一个点之间线性插值
        float ratio = (voltage - zero_voltage) / (voltage_table[0] - zero_voltage);
        *val = ratio * concentration_table[0]; // 0到5ppm之间
        return 3;
    }

    // 正常查表：找到电压所在的区间
    for (i = 0; i < TABLE_SIZE - 1; i++)
    {
        if (voltage >= voltage_table[i] && voltage <= voltage_table[i + 1])
        {
            // 线性插值计算浓度
            float voltage_diff = voltage_table[i + 1] - voltage_table[i];
            float concentration_diff = concentration_table[i + 1] - concentration_table[i];
            float ratio = (voltage - voltage_table[i]) / voltage_diff;

            *val = concentration_table[i] + ratio * concentration_diff;
            return 4;
        }
    }

    *val = concentration_table[TABLE_SIZE - 1];
    return 5;
}
#endif
