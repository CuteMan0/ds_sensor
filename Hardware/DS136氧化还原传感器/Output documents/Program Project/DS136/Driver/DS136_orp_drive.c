#include "DS136_orp_drive.h"

#include "stc32g.h"
#include "ADC_drive.h"
#include "LED_drive.h"

#include "log.h"

ADC_Handle_t adc0;

void ORP_Init(ORP_Handle_t* horp)
{
    adc_init(&adc0,0,3.3f);
    horp->polarity = 0;
    horp->vol = 0.0f;
}

float ORP_Read(ORP_Handle_t* horp)
{
    horp->vol = 2010 - 1333.33f * adc_get(&adc0);
    
    if(horp->vol < 0)
        horp->polarity = 1;
    else
        horp->polarity = 0;
//    LOG_DEBUG"polarity = %u\n",horp->polarity);

#define CLAMP_SIMPLE(val, lo, hi) \
    (((val) < (lo)) ? (lo) : (((val) > (hi)) ? (hi) : (val)))
    
    horp->vol = CLAMP_SIMPLE(horp->vol,-2000,2000);
    
    return (horp->vol - 7.5);
}