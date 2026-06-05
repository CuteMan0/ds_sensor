// 测试工作频率为 24 MHz
#include <string.h>
//------------------------------STC函数库文件--------------------------
#include "stc32g.h"
#include "stc32_stc8_usb.h"
#include "STC32G_GPIO.h"
#include "STC32G_NVIC.h"
#include "STC32G_Delay.h"
#include "STC32G_Switch.h"
//------------------------------用户文件------------------------------
#include "USB_CDC_drive.h"
#include "COMMON_drive.h"
#include "SYS_drive.h"
#include "KEY_drive.h"
#include "LED_drive.h"
#include "DS134_Color_drive.h"

extern unsigned long Adj_num;
extern unsigned char Transmission_mode_flag;

char test[200] = {0};

float dat[3];

void main(void)
{
    WTST = 0;  // 设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; // 扩展寄存器(XFR)访问使能
    CKCON = 0; // 提高访问XRAM速度

    // 基础功能初始化
//    DIS_LED_init();
//    DIS_LED_ALL_off();
    
//    KEY_GPIO_init(); 
    USB_CDC_Initialization(); // USB初始化

    //传感器初始化
    DS134_Init();
    
    EA = 1; // 开总中断

    while (DeviceState != DEVSTATE_CONFIGURED)
                    ; // 等待 USB 完成配置
    while (1)
    {
        DS134_GetRGB(dat);
        sprintf(test,"Red:%.2f%,Green:%.2f%,Blue:%.2f%\n",
        dat[0],dat[1],dat[2]);
        USB_SendData(&test,strlen(test));
            
        delay_ms(500);
    }
}
