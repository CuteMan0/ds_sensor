#include "avg_filter.h"

void AvgFilter_Init(avg_filter_t *filter, u16 *buffer, u8 len)
{
    u8 i;
    if (!filter || !buffer || len == 0)
    {
        return;
    }

    filter->buffer = buffer;
    filter->len = len;
    filter->index = 0;
    filter->sum = 0;
    filter->valid_count = 0;

    for (i = 0; i < len; i++)
    {
        buffer[i] = 0;
    }
}

u16 AvgFilter_Update(avg_filter_t *filter, u16 new_sample)
{
    if (!filter || !filter->buffer || filter->len == 0)
    {
        return 0;
    }

    filter->sum -= filter->buffer[filter->index];
    filter->buffer[filter->index] = new_sample;
    filter->sum += new_sample;

    filter->index = (filter->index + 1) % filter->len;

    if (filter->valid_count < filter->len)
    {
        filter->valid_count++;
    }

    return (u16)(filter->sum / filter->valid_count);
}
