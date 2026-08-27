//#include "drv_force_sensor.h"
//#include <stdio.h>

///*===========================================================================
// * 内部常量定义
// *===========================================================================*/

//#define FORCE_SENSOR_WEIGHT 2.0f

///*===========================================================================
// * 静态变量
// *===========================================================================*/

//static long code g_acc = 9806650;
//static bit s_calib_flag = 0;

///*===========================================================================
// * 内部函数声明
// *===========================================================================*/

//static void force_sensor_calc_ratio(ADC_SampleBuffer *padc);

///*===========================================================================
// * 公开接口实现
// *===========================================================================*/

///**
// * @brief  力传感器初始化
// * @note   从EEPROM读取校准数据，并初始化ADC0/ADC1通道的
// *         偏移和满量程校准值
// */
//void FORCE_SENSOR_init(void)
//{
//    EEPROM_ReadFullSampleADVal();
//    FORCE_SENSOR_set_calib(&ADC0_SampleBuffer, 2);
//    FORCE_SENSOR_set_calib(&ADC1_SampleBuffer, 2);
//}

///**
// * @brief  力传感器偏移/满量程校准设置
// * @param  padc: ADC采样缓冲指针
// * @param  mode: 校准模式
// *            0 - 仅累加偏移量，不保存到EEPROM
// *            1 - 满量程校准，计算比例系数
// *            2 - 从EEPROM读取偏移和满量程校准值
// *            3 - 保存偏移到EEPROM
// */
//void FORCE_SENSOR_set_calib(ADC_SampleBuffer *padc, u8 mode)
//{
//    if (mode == 0) // offset not save to eeprom
//    {
//        padc->ADTempOffset += padc->ADC_avg;
//        padc->pVoltageOffset->adc0Offset1 = padc->ADTempOffset;
//        return;
//    }
//    else if (mode == 1) // fullscale
//    {
//        padc->pVoltageOffset->adc0FullScale1 = padc->ADC_avg;
//        force_sensor_calc_ratio(padc);
//    }
//    else if (mode == 2) // read offset and fullscale cailbration
//    {
//        padc->ADTempOffset = padc->pVoltageOffset->adc0Offset1;
//        force_sensor_calc_ratio(padc);
//        return;
//    }
//    else if (mode == 3) // save offset to eeprom
//    {
//        padc->ADTempOffset = padc->pVoltageOffset->adc0Offset1;
//        force_sensor_calc_ratio(padc);
//    }
//    EEPROM_WriteFullSampleADVal();
//}

///**
// * @brief  力传感器标定处理流程
// * @note   当标定标志置位时，等待ADC平均值稳定后
// *         自动保存满量程校准值并关闭LED显示
// * @param  padc: ADC采样缓冲指针
// */
//void FORCE_SENSOR_calib_proc(ADC_SampleBuffer *padc)
//{
//    static int last_adc_avg = 0;

//    if (s_calib_flag)
//    {
//        if ((last_adc_avg < padc->ADC_avg + 1) && (last_adc_avg > padc->ADC_avg - 1))
//        {
//            s_calib_flag = 0;
//            FORCE_SENSOR_set_calib(padc, 1);
//            StopTimer(pvLEDShow_t);
//            LED_1 = 1;
//        }
//        last_adc_avg = padc->ADC_avg;
//        printf("ADC_avg = %d\r\n", padc->ADC_avg);
//    }
//}

///**
// * @brief  触发力传感器标定
// * @note   置位内部标定标志，配合 FORCE_SENSOR_calib_proc() 使用
// */
//void FORCE_SENSOR_trigger_calib(void)
//{
//    s_calib_flag = 1;
//}

///*===========================================================================
// * 内部函数实现
// *===========================================================================*/

///**
// * @brief  计算力传感器比例系数
// * @param  padc: ADC采样缓冲指针
// */
//static void force_sensor_calc_ratio(ADC_SampleBuffer *padc)
//{
//    padc->ratio = FORCE_SENSOR_WEIGHT * (float)g_acc;
//    padc->ratio = padc->ratio / (float)padc->pVoltageOffset->adc0FullScale1;
//}