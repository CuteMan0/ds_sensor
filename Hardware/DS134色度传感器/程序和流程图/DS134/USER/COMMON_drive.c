#include "config.h"

#include "STC32G_GPIO.h"
#include "STC32G_NVIC.h"
#include "STC32G_Delay.h"
#include "STC32G_Switch.h"
#include "LED_drive.h"
#include "COMMON_drive.h"
#include "stc32_stc8_usb.h"



//========================================================================
// 函数: void Wireless_transmission_USB_USB(void)
// 描述: USB_USB有线传输模式.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-07-01
void Wireless_transmission_USB_USB(void)
{
	
		if (bUsbOutReady) 	//当硬件接收完成上位机通过串口助手发送数据后会自动将 bUsbOutReady 置 1
												//接收的数据字节数保存在 OutNumber 变量中
												//接收的数据保存在 UsbOutBuffer 缓冲区
		{		
				USB_SendData(UsbOutBuffer,OutNumber); //使用 USB_SendData 库函数可向上位机发送数据
																							//这里的测试代码为将 接收数据原样返回				
				usb_OUT_done(); 	//处理完成接收的数据后	//调用 usb_OUT_done 准备 接收 下一笔数据
		}	
}







