#include "ds157_hr_meter.h"
// !!! Î´Íê³É

#if DS_SENSOR == 157

#include "bsp_max30102.h"

#include "sw_iic_drive.h"

#define FIFO_DEPTH 17

static u16 last_peak_index = 0;
static u16 sample_index = 0;

volatile u16 heart_rate = 0;

void ds_init(void)
{
    max30102_init();
}

void ds_update(u32 *dat)
{
    static u16 last_heart_rate = 0;
    u32 red_buf[10];
    u32 ir_buf[10];
    u8 count;

    max30102_read_fifo(red_buf, ir_buf, &count);
}
#endif