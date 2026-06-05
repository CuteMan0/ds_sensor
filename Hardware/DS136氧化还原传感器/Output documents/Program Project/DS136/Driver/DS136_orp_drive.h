#ifndef	__DS135_TUR_drive_H
#define	__DS135_TUR_drive_H

#include "type_def.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    u8 polarity; // sign vol is positive -- 0 or negative -- 1
    float vol;
}ORP_Handle_t;

void ORP_Init(ORP_Handle_t* horp);

float ORP_Read(ORP_Handle_t* horp);

#endif