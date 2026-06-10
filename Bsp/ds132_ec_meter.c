/*电导率传感器 0-20mS/cm*/
#include "ds132_ec_meter.h"

#if DS_SENSOR == 132

#include "stc32g.h"
#include "STC32G_Delay.h"
#include "STC32G_EEPROM.h"
#include "STC32G_GPIO.h"

#include "adc_drive.h"
#include "key_drive.h"
#include "led_drive.h"

#define VIN 0.19f
#define G_20MS 0.56f // 整流转平均值增益系数

// sw for range
sbit _C = P1 ^ 6;
sbit _B = P2 ^ 5;
sbit _A = P2 ^ 4;

#define SEL(x, y, z) \
    _C = (x);        \
    _B = (y);        \
    _A = (z)

#define REF_R0 SEL(0, 0, 0)
#define REF_R1 SEL(1, 1, 1)
#define REF_R2 SEL(1, 1, 0)
#define REF_R3 SEL(1, 0, 0)
#define REF_R4 SEL(1, 0, 1)
#define REF_R5 SEL(0, 1, 0)

u16 range_change_val[10] = {
    807,
    1179,
    385,
    1923,
    1129,
    2581,
    1787,
    3264,
    2469,
    3152};

float offset_val[5] = {
    0.30f,
    0.88f,
    1.4f,
    1.896f,
    2.43f};

static volatile float Q = 1.0f;   // 电导池常数
volatile float res_fb = 0.82f;    // 反馈电阻 kohm
volatile float offset_vol = 0.0f; // 减法器偏移量

static volatile u8 calibration_pending = 0; // 校准延迟处理变量
static volatile u8 led_flash = 0;           // LED闪烁标志

ADC_Handle_t adc0;
ADC_Handle_t adc1;

static void Auto_Switcher(void);
static void Scan_Key(void);

void ec_init(void)
{
    u8 tmp[2];

    adc_init(&adc0, 0, 3.3f);
    adc_init(&adc1, 1, 3.3f);

    P1_MODE_OUT_PP(GPIO_Pin_6); // P1.6设置为推挽输出(切换量程)
    P2_MODE_OUT_PP(GPIO_Pin_4); // P2.4设置为推挽输出(切换量程)
    P2_MODE_OUT_PP(GPIO_Pin_5); // P2.5设置为推挽输出(切换量程)

    P4_MODE_OUT_PP(GPIO_Pin_1); // P4.1设置为推挽输出(设置采样电阻)
    P4_MODE_OUT_PP(GPIO_Pin_0); // P4.0设置为推挽输出(设置采样电阻)

    REF_R0; // 设置基准为GND

    EEPROM_read_n(0, tmp, sizeof(tmp));
    if (tmp[0] != 0xff && tmp[1] != 0xff)
    {
        Q = (float)((u16)(tmp[0] << 8) + tmp[1]) / 1000;
    }
}

void ec_read(float *ec_val)
{
    float adc_vol = 0.0f;
    *ec_val = 0.0f;

    if (2 == flag_key) // 长按，开始EEPROM备份
    {
        flag_key = 0;
        adc_vol = (adc_get(&adc0) / 5.0f + offset_vol) / 2.0f; // 信号电压
        Q = (12.85f * res_fb * VIN * G_20MS) / adc_vol;
        calibration_pending = 1;
    }

    Auto_Switcher(); // 自动切换量程

    adc_vol = (adc_get(&adc0) / 5.0f + offset_vol) / 2.0f;
    *ec_val = adc_vol * Q / (res_fb * VIN * G_20MS); // k = Q/(R*|Vin|)*Vout
}

void ProcessCalibration(void)
{
    u16 tmp = 0xff;

    if (calibration_pending)
    {
        calibration_pending = 0;

        tmp = (u16)(Q * 1000);

        EEPROM_write_n(0,
                       (u8 *)&tmp,
                       sizeof(tmp));
        led_flash = 1;
    }
}

void Led_Task(void)
{
    static u8 cnt = 0;
    static bit on = 0;
    static u8 flash_cnt = 0;

    if (!led_flash)
        return;

    cnt++;

    if (cnt >= 10)
    {
        cnt = 0;

        on = !on;

        if (on)
        {

            DIS_LED_Just_One_Enable(1);
        }
        else
        {
            DIS_LED_ALL_off();
        }
    }

    if (cnt == 0)
    {
        flash_cnt++;

        if (flash_cnt >= 6)
        {
            flash_cnt = 0;
            led_flash = 0;

            DIS_LED_Just_One_Enable(1);
        }
    }
}

/************************************state machine************************************/
// states:
void Range_0(void); // 0-4
void Range_1(void); // 3.2-7.2
void Range_2(void); // 6.4-10.4
void Range_3(void); // 9.6-13.6
void Range_4(void); // 12.8-16.8
void Range_5(void); // 16.0-20.0

static eEvent eCurrentEvent = EVT_NO_EVENT;
static pfState pfCurrentState = Range_0;

// state transition table
StmRow_t stm[10] = {
    {Range_0, EVT_OverRange, Range_1},

    {Range_1, EVT_OverRange, Range_2},
    {Range_1, EVT_UnderRange, Range_0},

    {Range_2, EVT_OverRange, Range_3},
    {Range_2, EVT_UnderRange, Range_1},

    {Range_3, EVT_OverRange, Range_4},
    {Range_3, EVT_UnderRange, Range_2},

    {Range_4, EVT_OverRange, Range_5},
    {Range_4, EVT_UnderRange, Range_3},

    {Range_5, EVT_UnderRange, Range_4}};

static void Auto_Switcher(void)
{
    int idx = 0;
    int transitioned = 0;

    if (EVT_NO_EVENT != eCurrentEvent)
    {
        for (; idx < (int)(sizeof(stm) / sizeof(stm[0])); idx++)
        {
            if ((stm[idx].pSrcState == pfCurrentState) &&
                (stm[idx].eEvt == eCurrentEvent))
            {
                eCurrentEvent = EVT_NO_EVENT;
                pfCurrentState = stm[idx].pDestState;
                transitioned = 1;
                break;
            }
        }
    }
    if (NULL != pfCurrentState)
    {
        pfCurrentState();
    }
}

#define K2ADC(X) (u16)(res_fb * VIN * (X) / Q * 1240.9f)
void Range_0(void)
{
    u16 tmp;
    REF_R0;
    offset_vol = 0.0f;
    adc_get(&adc1);
    tmp = adc1.raw;

    if (tmp > range_change_val[0])
    {
        eCurrentEvent = EVT_OverRange;
    }
}
void Range_1(void)
{
    u16 tmp;
    REF_R1;
    offset_vol = offset_val[0];
    adc_get(&adc1);
    tmp = adc1.raw;

    if (tmp > range_change_val[1])
    {
        eCurrentEvent = EVT_OverRange;
    }
    if (tmp < range_change_val[2])
    {
        eCurrentEvent = EVT_UnderRange;
    }
}
void Range_2(void)
{
    u16 tmp;
    REF_R2;
    offset_vol = offset_val[1];
    adc_get(&adc1);
    tmp = adc1.raw;

    if (tmp > range_change_val[3])
    {
        eCurrentEvent = EVT_OverRange;
    }
    if (tmp < range_change_val[4])
    {
        eCurrentEvent = EVT_UnderRange;
    }
}
void Range_3(void)
{
    u16 tmp;
    REF_R3;
    offset_vol = offset_val[2];
    adc_get(&adc1);
    tmp = adc1.raw;

    if (tmp > range_change_val[5])
    {
        eCurrentEvent = EVT_OverRange;
    }
    if (tmp < range_change_val[6])
    {
        eCurrentEvent = EVT_UnderRange;
    }
}
void Range_4(void)
{
    u16 tmp;
    REF_R4;
    offset_vol = offset_val[3];
    adc_get(&adc1);
    tmp = adc1.raw;

    if (tmp > range_change_val[7])
    {
        eCurrentEvent = EVT_OverRange;
    }
    if (tmp < range_change_val[8])
    {
        eCurrentEvent = EVT_UnderRange;
    }
}
void Range_5(void)
{
    u16 tmp;
    REF_R5;
    offset_vol = offset_val[4];
    adc_get(&adc1);
    tmp = adc1.raw;

    if (tmp < range_change_val[9])
    {
        eCurrentEvent = EVT_UnderRange;
    }
}

#endif