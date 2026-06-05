#include "DS138_dC2H6O_drive.h"

#include "STC32G_ADC.h"
#include "STC32G_GPIO.h"

#define V_REF 3.3f
#define TABLE_SIZE (sizeof(voltage_table) / sizeof(voltage_table[0]))

// 电压与浓度对照表（按电压升序排列）
static float voltage_table[] = {
    0.24f, 0.53f, 0.70f, 0.78f, 0.85f, 0.91f, 0.95f, 0.98f, 1.00f, 1.03f,
    1.05f, 1.08f, 1.12f, 1.15f, 1.18f, 1.20f, 1.23f, 1.30f, 1.33f, 1.35f,
    1.40f, 1.45f
};

static const float concentration_table[] = {
    0.0f, 5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f,
    50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 120.0f, 150.0f, 160.0f, 200.0f,
    250.0f, 300.0f
};

static float zero_voltage_calibrated = 0.24f;  // 校准后的零点电压

void airC2H6O_Init(void)
{
    ADC_InitTypeDef ADC_InitSruct;
    
    P1_MODE_IN_HIZ(GPIO_Pin_0);		//P1.0设置为高阻输入
    
    ADC_InitSruct.ADC_SMPduty = 31;
    ADC_InitSruct.ADC_Speed = ADC_SPEED_2X16T;
    ADC_InitSruct.ADC_AdjResult = ADC_LEFT_JUSTIFIED;
    ADC_InitSruct.ADC_CsSetup = 0;
    ADC_InitSruct.ADC_CsHold = 1;
    
    ADC_Inilize(&ADC_InitSruct);
    ADC_PowerControl(ENABLE);
}

/**
 * @brief 零点校准函数（按下按钮时调用）
 * @return 1=校准成功, 0=校准失败（电压异常）
 */
uint8_t airC2H6O_CalibrateZero(void)
{
    // 读取当前电压作为新的零点
    float new_zero = adc_vol;
    
    // 检查电压是否合理（正常零点应该在0.2V-0.3V之间）
    if (new_zero < 0.1f || new_zero > 0.5f)
    {
        return 0;  // 校准失败，电压异常
    }
    
    // 只修改零点电压值
    zero_voltage_calibrated = new_zero;
    
    return 1;  // 校准成功
}

/**
 * @brief 获取当前零点电压
 * @return 零点电压值(V)
 */
float airC2H6O_GetZeroVoltage(void)
{
    return zero_voltage_calibrated;
}

/**
 * @brief 根据ADC电压值读取酒精浓度
 * @return 酒精浓度 (ppm)
 */
float airC2H6O_Read(void)
{
    float voltage = adc_vol;
    float zero_voltage = zero_voltage_calibrated;
    
    // 边界检查：低于校准后的零点
    if (voltage <= zero_voltage)
    {
        return concentration_table[0];
    }
    
    // 边界检查：高于最大电压
    if (voltage >= voltage_table[TABLE_SIZE - 1])
    {
        return concentration_table[TABLE_SIZE - 1];
    }
    
    // 特殊处理：电压在零点到第一个电压点之间
    if (voltage < voltage_table[0])
    {
        // 零点到第一个点之间线性插值
        float ratio = (voltage - zero_voltage) / (voltage_table[0] - zero_voltage);
        return ratio * concentration_table[0];  // 0到5ppm之间
    }
    
    // 正常查表：找到电压所在的区间
    for (int i = 0; i < TABLE_SIZE - 1; i++)
    {
        if (voltage >= voltage_table[i] && voltage <= voltage_table[i + 1])
        {
            // 线性插值计算浓度
            float voltage_diff = voltage_table[i + 1] - voltage_table[i];
            float concentration_diff = concentration_table[i + 1] - concentration_table[i];
            float ratio = (voltage - voltage_table[i]) / voltage_diff;
            
            return concentration_table[i] + ratio * concentration_diff;
        }
    }
    
    return concentration_table[TABLE_SIZE - 1];
}

