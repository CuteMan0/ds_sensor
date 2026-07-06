#include "STC32G_GPIO.h"
#include "stc32g.h"
#include "STC32G_Delay.h"
#include "stc32_stc8_usb.h"
#include "USB_CDC_drive.h"

char *USER_DEVICEDESC =NULL;				//设备描述
char *USER_PRODUCTDESC = NULL;			//产品描述
char *USER_STCISPCMD = "@STCISP#";	//不停电自动 ISP 下载命令


//========================================================================
// 函数: void USB_CDC_GPIO_config(void)
// 描述: P3.0/P3.1 和 USB 的 D +-/ 共用 PIN 脚，配置I/O脚.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void USB_CDC_GPIO_config(void)
{	
	P3_MODE_IN_HIZ(GPIO_Pin_0|GPIO_Pin_1)	//需要将 P3.0/P3.1设置为高阻输入模式
}

//========================================================================
// 函数: void USB_CDC_Initialization(void)
// 描述: USB_CDC初始化.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void USB_CDC_Initialization(void)
{
	
	USB_CDC_GPIO_config();
	IRC48MCR = 0x80; //使能内部 48M 的 USB 专用 IRC
	while (!(IRC48MCR & 0x01));
	USBCLK = 0x00; //设置 USB 时钟源为内部 48M 的 USB 专用 IRC
	USBCON = 0x90; //使能 USB 功能
		
	usb_init(); //调用 USB CDC 初始化库函数
		
	EUSB = 1; //使能 USB 中断	
}

//========================================================================
// 函数: void USB_CDC_Senddata(unsigned long count)
// 描述: USB_CDC测试.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void USB_CDC_Senddata(unsigned long count)
{		
		//if (bUsbOutReady) 	//当硬件接收完成上位机通过串口助手发送数据后会自动将 bUsbOutReady 置 1
												//接收的数据字节数保存在 OutNumber 变量中
												//接收的数据保存在 UsbOutBuffer 缓冲区
	//	{
				OutNumber=sprintf(UsbOutBuffer, "50N力传感器(N):%d\n",count);
				USB_SendData(UsbOutBuffer,OutNumber); //使用 USB_SendData 库函数可向上位机发送数据
																							//这里的测试代码为将 接收数据原样返回					
				usb_OUT_done(); 	//处理完成接收的数据后	//调用 usb_OUT_done 准备 接收 下一笔数据
		//}
}
//========================================================================
// 函数: void USB_CDC_Test(void)
// 描述: USB_CDC测试.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void USB_CDC_Test(void)
{
		unsigned char iii;
//		while (DeviceState != DEVSTATE_CONFIGURED); //等待 USB 完成配置
		if (bUsbOutReady) 	//当硬件接收完成上位机通过串口助手发送数据后会自动将 bUsbOutReady 置 1
												//接收的数据字节数保存在 OutNumber 变量中
												//接收的数据保存在 UsbOutBuffer 缓冲区
		{
				if (UsbOutBuffer[0] == 'K') 
				{
						UsbOutBuffer[0] = 'G';	
						UsbOutBuffer[1] = 'O';	
						UsbOutBuffer[2] = 'L';	
						UsbOutBuffer[3] = 'D';								
						UsbOutBuffer[4] = 'E';	
						UsbOutBuffer[5] = 'N';		
						for(iii=0;iii<6;iii++)
						{		
								USB_SendData((UsbOutBuffer+iii),OutNumber); //使用 USB_SendData 库函数可向上位机发送数据
						}		
						USB_SendData(UsbOutBuffer,OutNumber); //使用 USB_SendData 库函数可向上位机发送数据
				}
				else
				{
						USB_SendData(UsbOutBuffer,OutNumber); //使用 USB_SendData 库函数可向上位机发送数据
																							//这里的测试代码为将 接收数据原样返回
				}					
				usb_OUT_done(); 	//处理完成接收的数据后	//调用 usb_OUT_done 准备 接收 下一笔数据
		}	
}
/*
BOOL usb_OUT_callback()
{
    USB_SendData(UsbOutBuffer, OutNumber);
    return 1;
}
*/



