#ifndef __DS_SENSOR_H
#define __DS_SENSOR_H

#include "task_scheduler.h"
#include "filter.h"

#define DS_SENSOR 144

/* ============================================================
 * 共享全局变量（由某个 .c 文件定义）
 * ============================================================ */
extern volatile float dat_for_printf;
extern avg_filter_t filter;
#define NUM_BUF_AVG 8
extern avgf_data_t buffer[NUM_BUF_AVG];

/* ============================================================
 * 统一接口 — 每个 dsXX 文件必须实现以下 4 个函数：
 *   ds_init()    — 传感器初始化（main 启动时调用一次）
 *   ds_update()  — 传感器数据采集 + 滤波（task_sensor 调用）
 *   ds_printf()  — 格式化打印（task_printf 调用，支持多值）
 *   ds_calib()   — 校准保存（task_calibration_save 调用，无需校准则空函数）
 * ============================================================ */
void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

/* ============================================================
 * 按 DS_SENSOR 宏包含具体传感器头文件
 * ============================================================ */
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
#if DS_SENSOR == 134 /*色度传感器*/
// ..\ds_sensor\Hardware\DS134色度传感器\程序和流程图\DS134
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
#if DS_SENSOR == 141 /*循环式二氧化碳传感器*/
#include "ds141_co2_meter.h"
#endif
#if DS_SENSOR == 144 /*溶解氧传感器*/
#include "ds144_do_meter.h"
#endif
#if DS_SENSOR == 145 /*溶解二氧化碳传感器*/
#include "ds145_dco2_meter.h"
#endif
#if DS_SENSOR == 151 /*土壤温度传感器*/
#include "ds151_soil_temp_meter.h"
#endif
#if DS_SENSOR == 153 /*氢气传感器*/
#include "ds153_h2_meter.h"
#endif
#if DS_SENSOR == 157 /*心率传感器*/
// ..\ds_sensor\Hardware\DS157心率传感器\程序和流程图\DS157
#endif
#if DS_SENSOR == 159 /*呼吸率传感器*/
// ..\ds_sensor\Hardware\DS159呼吸率传感器\程序和流程图\DS159
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
#include "ds163_nh3_meter.h"
#endif
#if DS_SENSOR == 164 /*氯气传感器*/
#include "ds164_cl2_meter.h"
#endif
#if (DS_SENSOR == 165 || DS_SENSOR == 166 || DS_SENSOR == 167 || DS_SENSOR == 168 || DS_SENSOR == 169 || DS_SENSOR == 170) /*离子传感器*/
#include "ds165_170_ise_meter.h"
#endif
#if DS_SENSOR == 172 /*滴定计数器*/
#include "ds172_titration_meter.h"
#endif

#endif