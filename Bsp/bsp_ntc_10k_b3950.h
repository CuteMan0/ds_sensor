#ifndef __BSP_NTC_10K_B3950_H
#define __BSP_NTC_10K_B3950_H

#include "type_def.h"

void bsp_ntc_10k_b3950_init(void);
float bsp_ntc_10k_b3950_temperature_get(u16 adc_value);

#endif /* __BSP_NTC_10K_B3950_H */