#ifndef __AVG_FILTER_H__
#define __AVG_FILTER_H__

#include <intrins.h>
#include "type_def.h"


/*示例
avg_filter_t xx_filter;
u16          xx_buffer[];
AvgFilter_Init(&xx_filter,xx_buffer,sizeof(xx_buffer)/2);
*/

typedef struct {
    u16 *buffer;     // 数据缓存指针
    u8  len;        // 总窗口大小
    u8  index;      // 当前写入位置
    u8  valid_count;// 有效样本数量
    u32  sum;        // 当前窗口总和
}avg_filter_t;

void AvgFilter_Init(avg_filter_t *filter, u16 *buffer, u8 len);
u16 AvgFilter_Update(avg_filter_t *filter, u16 new_sample);

#endif
