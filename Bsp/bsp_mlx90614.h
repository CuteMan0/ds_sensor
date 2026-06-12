#ifndef __BSP_MLX90614_H
#define __BSP_MLX90614_H

#include "type_def.h"

#define MLX90614_DEV_ADDRESS 0x5A
#define MLX90614_REG_TOBJ1 0x07

u8 mlx_init(void);
void mlx_getVal(float *dat);

#endif