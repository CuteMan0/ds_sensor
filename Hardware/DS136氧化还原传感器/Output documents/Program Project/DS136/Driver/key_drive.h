#ifndef __KEY_DRIVER_H__
#define __KEY_DRIVER_H__

#include "stc32g.h"
#include "type_def.h"

void gpio_init(void);
u8 gpio_ReadPin(u8 port, u8 pin);

#endif