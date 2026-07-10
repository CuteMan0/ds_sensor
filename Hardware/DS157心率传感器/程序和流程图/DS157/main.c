// 测试工作频率为 24 MHz
#include "stc32g.h"

#include <stdio.h>
#include <math.h>

#include "LED_drive.h"
#include "USB_CDC_drive.h"
#include "STC32G_Delay.h"
#include "ADC_drive.h"

#include "log.h"

ADC_Handle_t adc0;

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
    
    adc_init(&adc0,0,3.3);
    
    while (1)
    {
        printf("%f\n",adc_get(&adc0));
        delay_ms(20);
        

    }
}
