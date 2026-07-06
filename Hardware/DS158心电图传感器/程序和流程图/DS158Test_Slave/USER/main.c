// 测试工作频率为 24 MHz
#include <string.h>
//------------------------------STC函数库文件--------------------------
#include "stc32g.h"
#include "stc32_stc8_usb.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"
//------------------------------用户文件------------------------------
#include "USB_CDC_drive.h"
#include "KEY_drive.h"
#include "LED_drive.h"
#include "adc_drive.h"

#include "log.h"
#include "iir.h"


void main(void)
{
    u16 ECG;
    ADC_Handle_t adc0;

    float fs = 1000.0f;      // 采样频率1kHz
    float f0 = 50.0f;        // 50Hz陷波
    float r = 0.9f;         // 极半径
    int num_samples = 1000;  // 采样点数
    
    float t = 0.0f;
    float dt = 1.0f / fs;
    
    float input,output,attenuation;
    
    NotchFilterFixed filter1;
    NotchFilterFixed filter2;
    NotchFilterFixed filter3;
    notch_fixed_init(&filter1, fs, f0, r);
    notch_fixed_init(&filter2, fs, f0, r);
    notch_fixed_init(&filter3, fs, f0, r);
    WTST = 0;  // 设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; // 扩展寄存器(XFR)访问使能
    CKCON = 0; // 提高访问XRAM速度
  
    USB_CDC_Initialization();

    adc_init(&adc0,0,3.3f);
    EA = 1; 
    while (DeviceState != DEVSTATE_CONFIGURED);
    
    while (1)
    {
        adc_get(&adc0);
        input = adc_to_q15(adc0.raw);
        output = notch_fixed_process(&filter1, input);
        output = notch_fixed_process(&filter2, output);
        output = notch_fixed_process(&filter3, output);
        printf("%.4f,%.4f\n", 
               input/32768.0f, output/32768.0f);
    }
}
