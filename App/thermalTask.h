#ifndef __THERMALTASK_H
#define __THERMALTASK_H

#include "type_def.h"

#define Dis_on 0
#define Dis_off 1

extern u8 channel_temp[5];
extern u32 loop_counter;

void led_check(void);
void sample_all_channels(void);
void update_led_display(u8 temperature);
void display_time_divider(void);

#endif