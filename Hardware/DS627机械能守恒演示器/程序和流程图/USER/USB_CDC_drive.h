#ifndef	__USB_CDC_drive_H
#define	__USB_CDC_drive_H

#define	FOSC 24000000 UL //ISP 下载时需将工作频率设置为 24MHz

//extern char *USER_DEVICEDESC =NULL;				//设备描述
//extern char *USER_PRODUCTDESC = NULL;			//产品描述
//extern char *USER_STCISPCMD = "@STCISP#";	//不停电自动 ISP 下载命令

void USB_CDC_GPIO_config(void);
void USB_CDC_Initialization(void);
void USB_CDC_Senddata(unsigned long count);
void USB_CDC_Test(void);


#endif
