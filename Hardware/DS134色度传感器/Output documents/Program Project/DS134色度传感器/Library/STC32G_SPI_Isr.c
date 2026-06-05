#include	"STC32G_SPI.h"
#include "COMMON_drive.h"
#include "LED_drive.h"
#include	"STC32G_UART.h"

//========================================================================
//                               本地变量声明
//========================================================================

u8 	SPI_RxCnt;

//========================================================================
// 函数: SPI_ISR_Handler
// 描述: SPI中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2020-09-23
//========================================================================
void SPI_ISR_Handler() interrupt SPI_VECTOR
{		
		BELL =~BELL;	//测试用			
		if(MSTR)	//主机模式
		{
				B_SPI_Busy = 0;		//主机模式，发送完成后发送忙标志清零	
		}
		else							//从机模式
		{		
				if(SPI_RxCnt >= SPI_BUF_LENTH)	
				{			
						SPI_RxCnt = 0;
				}
				SPI_RxBuffer[SPI_RxCnt++] = SPDAT;
				SPI_RxTimerOut = 5;					
		}
		SPI_ClearFlag();	//清0 SPIF和WCOL标志
}

