#ifndef __DRV_MLX90614_H
#define __DRV_MLX90614_H

#include "type_def.h"

#define MLX90614_DEV_ADDRESS 0x5A
#define MLX90614_REG_TOBJ1 0x07

u8 mlx90614_init(void);
void mlx90614_getVal(float *dat);

#endif