// 测试工作频率为 24 MHz
#include "stc32g.h"

#include <stdio.h>
#include <math.h>

#include "LED_drive.h"
#include "USB_CDC_drive.h"
#include "STC32G_Delay.h"

#include "log.h"

void main(void)
{
    WTST = 0;
    EAXFR = 1;
    CKCON = 0;

    USB_CDC_Initialization();
    EA = 1;
    while (DeviceState != DEVSTATE_CONFIGURED);
    
    led_init();
    fft_init();
    max30102_init();
    
    while (1)
    {
        task_update();
        task_sensor();
    }
}
