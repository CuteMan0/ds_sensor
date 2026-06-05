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
#include "DS144_do_drive.h"

extern unsigned long Adj_num;
extern unsigned char Transmission_mode_flag;

char test[100] = {0};
float Do = 0.0f;

void main(void)
{
    WTST = 0;  // 设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; // 扩展寄存器(XFR)访问使能
    CKCON = 0; // 提高访问XRAM速度

    // 基础功能初始化
//    DIS_LED_init();
//    KEY_GPIO_init();
    USB_CDC_Initialization(); // USB初始化

    // Do传感器初始化
    Do_Init();

    EA = 1; // 开总中断

    while (DeviceState != DEVSTATE_CONFIGURED)
                    ; // 等待 USB 完成配置
    while (1)
    {
        Do = Do_Read();

//        if (bUsbOutReady) // 当硬件接收完成上位机通过串口助手发送数据后会自动将 bUsbOutReady 置 1
//                                    // 接收的数据字节数保存在 OutNumber 变量中
//                                    // 接收的数据保存在 UsbOutBuffer 缓冲区
//        {
            sprintf(test,"DO:%.2f\n",Do);
            USB_SendData(&test,strlen(test)); // 使用 USB_SendData 库函数可向上位机发送数据
                                                     // 这里的测试代码为将 接收数据原样返回
//            usb_OUT_done(); // 处理完成接收的数据后	//调用 usb_OUT_done 准备 接收 下一笔数据
//        }
        delay_ms(100);
    }
}
