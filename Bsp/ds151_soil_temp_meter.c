#include "ds151_soil_temp_meter.h"

#if DS_SENSOR == 151

#include "bsp_ds18b20.h"
#include "STC32G_Delay.h"

void ds_init(void)
{
    ds18b20_init();
}

void ds_update(float *dat)
{
    ds18b20_start_conversion(DS18B20_RES_12BIT);
    delay_ms(750);
    ds18b20_read_temperature(dat);
}

#endif