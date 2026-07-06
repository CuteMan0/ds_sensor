#include "config.h"
#include "SYS_drive.h"


unsigned char Transmission_mode_flag=0;


 
unsigned long Char_Array_To_Long(char *array) 
{
		unsigned char pos;
		unsigned long Temp_Data=0;
		
		for(pos=0;pos<4;pos++)
		{	
			Temp_Data<<=8;
			Temp_Data=Temp_Data+array[pos] ;		
		}
	
//		Temp_Data<<=8;		
//		Temp_Data=Temp_Data+array[0];		
//		Temp_Data<<=8;	
//		Temp_Data=Temp_Data+array[1];
//		Temp_Data<<=8;
//		Temp_Data=Temp_Data+array[2];
//		Temp_Data<<=8;
//		Temp_Data=Temp_Data+array[3];

    return Temp_Data;
}
 

void Long_To_Char_Array(long value, unsigned char result[4])
{
		unsigned char pos;
    for (pos= 0; pos < 4; pos++) 
		{
        result[pos] = (value >> (pos * 8)) & 0xFF;
    }
}

// 函数用于判断两个数组是否相同
unsigned char areArraysEqual(unsigned char arr1[], unsigned char arr2[], unsigned char Array_size)
{
		unsigned char pos;	
    for (pos = 0; pos < Array_size; pos++) 
		{
        if (arr1[pos] != arr2[pos]) 
				{
            return 0; // 如果有任何一个元素不匹配，返回0
        }
    }
    return 1; // 如果所有元素都匹配，返回1
}





