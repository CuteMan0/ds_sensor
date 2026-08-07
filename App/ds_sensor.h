#ifndef __DS_SENSOR_H
#define __DS_SENSOR_H

/**
 * @file    ds_sensor.h
 * @brief   传感器统一接口头文件
 * @note    通过修改 DS_SENSOR 宏值切换传感器型号，编译时自动包含对应的驱动头文件。
 *          每个传感器驱动文件（dsXX_xxx_meter.c）需实现以下四个统一接口：
 *            - ds_init()      : 传感器初始化
 *            - ds_update()    : 数据采集 + 滤波处理
 *            - ds_printf()    : 格式化打印输出
 *            - ds_calib()     : 校准保存
 */

/* ============================================================
 * 头文件包含
 * ============================================================ */
#include <stdio.h>
#include <string.h>

#include "stc32_stc8_usb.h"

#include "task_scheduler.h"
#include "filter.h"

/* ============================================================
 * 传感器型号选择
 *   修改此宏值即可切换不同传感器型号：
 *     112 = 红外温度传感器
 *     131 = pH 传感器
 *     132 = 电导率传感器 (0-20mS/cm)
 *     133 = 多量程电导率传感器 (0-20mS/cm, 0-2mS/cm, 0-0.2mS/cm)
 *     134 = 色度传感器 (硬件设计阶段，待实现)
 *     135 = 浊度传感器
 *     136 = 氧化还原传感器 (ORP)
 *     137 = 二氧化硫传感器
 *     138 = 酒精气体传感器
 *     139 = 氧气传感器
 *     141 = 循环式二氧化碳传感器
 *     144 = 溶解氧传感器
 *     145 = 溶解二氧化碳传感器
 *     151 = 土壤温度传感器
 *     153 = 氢气传感器
 *     157 = 盐分传感器 (硬件设计阶段，待实现)
 *     159 = 氨气传感器 (硬件设计阶段，待实现)
 *     160 = 二氧化氮传感器
 *     161 = 一氧化碳传感器
 *     162 = 甲烷传感器
 *     163 = 氨气传感器
 *     164 = 氯气传感器
 *     165~170 = 离子传感器
 *     172 = 滴定传感器
 * ============================================================ */
#define DS_SENSOR 151

/* ============================================================
 * 全局变量声明（定义在 task.c / main.c）
 * ============================================================ */
extern volatile float dat_for_printf;    /**< 待打印的传感器数据 */

extern avg_filter_t filter;              /**< 滑动平均滤波器实例 */
#define NUM_BUF_AVG 8                    /**< 滤波缓冲区大小 */
extern avgf_data_t buffer[NUM_BUF_AVG];  /**< 滤波数据缓冲区 */

extern char usb_txbuf[64];              /**< USB 发送缓冲区 */
extern u8 usb_send_flag;                /**< USB 发送完成标志 */
extern u8 usb_send_len;                 /**< USB 发送数据长度 */

/* ============================================================
 * 传感器统一接口函数
 *   每个传感器驱动文件（dsXX_xxx_meter.c）必须实现以下四个函数：
 *
 *   ds_init()     — 传感器初始化，在 main() 启动时调用一次
 *   ds_update()   — 传感器数据采集 + 滤波，由 task_sensor 任务周期调用
 *   ds_printf()   — 格式化打印输出，由 task_printf 任务周期调用，支持多值打印
 *   ds_calib()    — 校准保存，由 task_calibration_save 任务调用，仅在触发校准后执行
 * ============================================================ */
void ds_init(void);
void ds_update(void);
void ds_printf(void);
void ds_calib(void);

/* ============================================================
 * 根据 DS_SENSOR 宏值条件包含对应传感器驱动头文件
 * ============================================================ */

/** @brief 需要 LED 闪烁指示的传感器型号（校准状态下使用） */
#define USE_BLINK (DS_SENSOR == 132 || DS_SENSOR == 133 || DS_SENSOR == 138 || DS_SENSOR == 144 || DS_SENSOR == 165)

/* ---- 红外温度传感器 (DS112) ---- */
#if DS_SENSOR == 112
#include "ds112_fir_meter.h"
#endif

/* ---- pH 传感器 (DS131) ---- */
#if DS_SENSOR == 131
#include "ds131_ph_meter.h"
#endif

/* ---- 电导率传感器 (DS132) ---- */
#if DS_SENSOR == 132
#include "ds132_ec_meter.h"
#endif

/* ---- 多量程电导率传感器 (DS133) ---- */
#if DS_SENSOR == 133
#include "ds133_multi_ec_meter.h"
#endif

/* ---- 色度传感器 (DS134, 硬件设计阶段) ---- */
#if DS_SENSOR == 134
// ..\\ds_sensor\\Hardware\\DS134色度传感器\\产品原理图\\DS134
#endif

/* ---- 浊度传感器 (DS135) ---- */
#if DS_SENSOR == 135
#include "ds135_tur_meter.h"
#endif

/* ---- 氧化还原传感器 (DS136) ---- */
#if DS_SENSOR == 136
#include "ds136_orp_meter.h"
#endif

/* ---- 二氧化硫传感器 (DS137) ---- */
#if DS_SENSOR == 137
#include "ds137_so2_meter.h"
#endif

/* ---- 酒精气体传感器 (DS138) ---- */
#if DS_SENSOR == 138
#include "ds138_airC2H6O_meter.h"
#endif

/* ---- 氧气传感器 (DS139) ---- */
#if DS_SENSOR == 139
#include "ds139_o2_meter.h"
#endif

/* ---- 循环式二氧化碳传感器 (DS141) ---- */
#if DS_SENSOR == 141
#include "ds141_co2_meter.h"
#endif

/* ---- 溶解氧传感器 (DS144) ---- */
#if DS_SENSOR == 144
#include "ds144_do_meter.h"
#endif

/* ---- 溶解二氧化碳传感器 (DS145) ---- */
#if DS_SENSOR == 145
#include "ds145_dco2_meter.h"
#endif

/* ---- 土壤温度传感器 (DS151) ---- */
#if DS_SENSOR == 151
#include "ds151_soil_temp_meter.h"
#endif

/* ---- 氢气传感器 (DS153) ---- */
#if DS_SENSOR == 153
#include "ds153_h2_meter.h"
#endif

/* ---- 盐分传感器 (DS157, 硬件设计阶段) ---- */
#if DS_SENSOR == 157
// ..\\ds_sensor\\Hardware\\DS157盐分传感器\\产品原理图\\DS157
#endif

/* ---- 氨气传感器 (DS159, 硬件设计阶段) ---- */
#if DS_SENSOR == 159
// ..\\ds_sensor\\Hardware\\DS159氨气传感器\\产品原理图\\DS159
#endif

/* ---- 二氧化氮传感器 (DS160) ---- */
#if DS_SENSOR == 160
#include "ds160_no2_meter.h"
#endif

/* ---- 一氧化碳传感器 (DS161) ---- */
#if DS_SENSOR == 161
#include "ds161_co_meter.h"
#endif

/* ---- 甲烷传感器 (DS162) ---- */
#if DS_SENSOR == 162
#include "ds162_ch4_meter.h"
#endif

/* ---- 氨气传感器 (DS163) ---- */
#if DS_SENSOR == 163
#include "ds163_nh3_meter.h"
#endif

/* ---- 氯气传感器 (DS164) ---- */
#if DS_SENSOR == 164
#include "ds164_cl2_meter.h"
#endif

/* ---- 离子传感器 (DS165~DS170) ---- */
#if (DS_SENSOR == 165 || DS_SENSOR == 166 || DS_SENSOR == 167 || DS_SENSOR == 168 || DS_SENSOR == 169 || DS_SENSOR == 170)
#include "ds165_170_ise_meter.h"
#endif

/* ---- 滴定传感器 (DS172) ---- */
#if DS_SENSOR == 172
#include "ds172_titration_meter.h"
#endif

#endif /* __DS_SENSOR_H */
