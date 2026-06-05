#include "usb_cdc_drive.h"
#include "stc32g.h"
#include "STC32G_GPIO.h"
#include "stc32_stc8_usb.h"

char *USER_DEVICEDESC =NULL;				//设备描述
char *USER_PRODUCTDESC = NULL;			//产品描述
char *USER_STCISPCMD = "@STCISP#";	//不停电自动 ISP 下载命令

//========================================================================
// 函数: void USB_CDC_Initialization(void)
// 描述: USB_CDC初始化.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void USB_CDC_Initialization(void)
{
	P3_MODE_IN_HIZ(GPIO_Pin_0|GPIO_Pin_1)
	IRC48MCR = 0x80; //使能内部 48M 的 USB 专用 IRC
	while (!(IRC48MCR & 0x01));
	USBCLK = 0x00; //设置 USB 时钟源为内部 48M 的 USB 专用 IRC
	USBCON = 0x90; //使能 USB 功能
		
	usb_init(); //调用 USB CDC 初始化库函数
		
	EUSB = 1; //使能 USB 中断	
}