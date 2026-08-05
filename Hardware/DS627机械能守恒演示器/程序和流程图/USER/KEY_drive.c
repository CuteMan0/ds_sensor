#include "STC32G_GPIO.h"
#include "STC32G_DMA.h"
#include "STC32G_NVIC.h"
#include "STC32G_LCM.h"
#include "STC32G_Delay.h"
#include "STC32G_Switch.h"
#include "KEY_drive.h"
#include "LED_drive.h"

//========================================================================
// 函数: void KEY_GPIO_config(void)
// 描述: 按键_IO口配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void KEY_GPIO_config(void)
{
	P3_MODE_IN_HIZ(GPIO_Pin_2);		//P3.2设置为准双向口		
	P3_PULL_UP_ENABLE(GPIO_Pin_2);//P3.2上拉使能	
}

//========================================================================
// 函数: void KEY_GPIO_init(void)
// 描述: 按键_IO口配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void KEY_GPIO_init(void)
{
	KEY_GPIO_config();	
}

//========================================================================
// 函数: void KEY_GPIO_config(void)
// 描述: 按键_IO口配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void KEY_Test(void)
{
	if(HOME_key==0)
	{
		delay_ms(10);
		 if(HOME_key==0)
		 {
				while(HOME_key==0);

		 }  
	}
}





