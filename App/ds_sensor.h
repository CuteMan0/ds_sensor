#ifndef __DS_SENSOR_H
#define __DS_SENSOR_H

#define DS_SENSOR 161

#if DS_SENSOR == 112 /*红外温度传感器 */
#include "ds112_fir_meter.h"
#endif
#if DS_SENSOR == 131 /*ph传感器 */
#include "ds131_ph_meter.h"
#endif
#if DS_SENSOR == 132 /*电导率传感器 0-20mS/cm*/
#include "ds132_ec_meter.h"
#endif
#if DS_SENSOR == 133 /*多量程电导率传感器 0-20mS/cm、0-2mS/cm、0-0.2mS/cm*/
#include "ds133_multi_ec_meter.h"
#endif
#if DS_SENSOR == 135 /*浊度传感器*/
#include "ds135_tur_meter.h"
#endif
#if DS_SENSOR == 136 /*氧化还原传感器*/
#include "ds136_orp_meter.h"
#endif
#if DS_SENSOR == 137 /*二氧化硫传感器*/
#include "ds137_so2_meter.h"
#endif
#if DS_SENSOR == 138 /*气体酒精传感器*/
#include "ds138_airC2H6O_meter.h"
#endif
#if DS_SENSOR == 139 /*氧气传感器*/
#include "ds139_o2_meter.h"
#endif
#if DS_SENSOR == 144 /*溶解氧传感器*/
#include "ds144_do_meter.h"
#endif
#if DS_SENSOR == 145 /*溶解二氧化碳传感器*/
#include "ds145_dco2_meter.h"
#endif
#if DS_SENSOR == 153 /*氢气传感器*/
#include "ds153_h2_meter.h"
#endif
#if DS_SENSOR == 160 /*二氧化氮传感器*/
#include "ds160_no2_meter.h"
#endif
#if DS_SENSOR == 161 /*一氧化碳传感器*/
#include "ds161_co_meter.h"
#endif
#if DS_SENSOR == 162 /*甲烷传感器*/
#include "ds162_ch4_meter.h"
#endif
#if DS_SENSOR == 163 /*氨气传感器*/
#include "ds163_nh4_meter.h"
#endif
#if DS_SENSOR == 164 /*氯气传感器*/
#include "ds164_cl2_meter.h"
#endif

#endif