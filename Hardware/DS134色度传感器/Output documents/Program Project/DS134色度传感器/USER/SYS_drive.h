#ifndef	__SYS_drive_H
#define	__SYS_drive_H


//#include "intrins.h"
//#include "stdio.h"
//#include "string.h"
//#include "stc32g.h"

#define SPI_SPI_mode     0
#define SPI_UART1_mode   1
#define SPI_UART2_mode   2
#define SPI_USB_mode     3	
#define USB_USB_mode     4	


unsigned long Char_Array_To_Long(char *array);
void Long_To_Char_Array(long value, unsigned char result[4]);
unsigned char areArraysEqual(unsigned char arr1[], unsigned char arr2[], unsigned char Array_size);

#endif

