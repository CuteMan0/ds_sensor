// 测试工作频率为 24 MHz
#include "stc32g.h"
#include <stdio.h>

#include "USB_CDC_drive.h"
#include "LED_drive.h"
#include "DS136_orp_drive.h"

#include "log.h"
#include "task_scheduler.h"
#include "avg_filter.h"

#define NUM_BUF_AVG 6
avg_filter_t filter;
avgf_data_t  buffer[NUM_BUF_AVG];

ORP_Handle_t orp;

void task_sensor(void)
{
    ORP_Read(&orp);
}

void task_printf(void)
{
    led_flashing(1);
    printf("ORG: %.3f\n", avg_filter_update(&filter, orp.vol));
}

void main(void)
{
    WTST = 0;
    EAXFR = 1;
    CKCON = 0;

    USB_CDC_Initialization();
    EA = 1;
    while (DeviceState != DEVSTATE_CONFIGURED);
    
    led_init();
    avg_filter_init(&filter, buffer, NUM_BUF_AVG);

    ORP_Init(&orp);
    
    task_scheduler_init();
    task_register(task_sensor, 50, 1);
    task_register(task_printf, 300, 1);
    
    while (1)
    {
        task_scheduler_run();
    }
}

void Timer0_ISR_Handler(void) interrupt TMR0_VECTOR
{
    task_scheduler_tick_isr();
}
