/*多量程电导率传感器 0-20mS/cm、0-2mS/cm、0-0.2mS/cm*/
#include "ds133_multi_ec_meter.h"

#if DS_SENSOR == 133

#include "stc32g.h"
#include "STC32G_Delay.h"
#include "STC32G_EEPROM.h"
#include "STC32G_GPIO.h"

#include "adc_drive.h"
#include "key_drive.h"
#include "led_drive.h"

#define VIN 0.19f
#define AVG_G 0.56f // 整流转平均值增益系数

// sw for range
sbit _C = P1 ^ 6;
sbit _B = P2 ^ 5;
sbit _A = P2 ^ 4;
// sw for Rs
sbit IN1 = P4 ^ 1;
sbit IN2 = P4 ^ 0;

#define SEL(x, y, z) \
    _C = (x);        \
    _B = (y);        \
    _A = (z)

#define CTL(x, y) \
    IN2 = (x);    \
    IN1 = (y)

#define REF_R0 SEL(0, 0, 0) // X0    0V
#define REF_R1 SEL(1, 1, 1) // X7  0.299V
#define REF_R2 SEL(1, 1, 0) // X6 0.888V
#define REF_R3 SEL(1, 0, 0) // X4  1.418V
#define REF_R4 SEL(1, 0, 1) // X5  1.6455V 异常值
#define REF_R5 SEL(0, 1, 0) // X2  2.507V

#define COM_OFF CTL(0, 0)
#define COM_NO0 CTL(0, 1)
#define COM_NO1 CTL(1, 0)
#define COM_NO2 CTL(1, 1)

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

u8 flag_backup = 0;
// 校准延迟处理变量
static volatile u8 calibration_pending = 0;

ADC_Handle_t adc0;
ADC_Handle_t adc1;

static void Set_EC_Range(EC_Range_t range);
static void Auto_Switcher(void);
static void EC_Range_Manager(void);
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
    Set_EC_Range(EC_RANGE_20MS);
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

    if (1 == flag_key) // 开始EEPROM备份
    {
        flag_key = 0;
#define K 12.85f                                  // mS/cm
        adc_vol = adc_get(&adc1) / 2;             // 获取最初处理的信号电压
        Q = (K * res_fb * VIN * AVG_G) / adc_vol; // new Q value
    }

    Auto_Switcher(); // 自动切换量程
    EC_Range_Manager();

    adc_vol = (adc_get(&adc0) / 5.0f + offset_vol) / 2.0f;
    *ec_val = adc_vol * Q / (res_fb * VIN * AVG_G); // k = Q/(R*|Vin|)*Vout,Vin = Vp * 2/pi
}

void ProcessCalibration(void)
{
    u8 tmp[2];
    if (calibration_pending)
    {
        calibration_pending = 0;

        // 写入EEPROM（这里过于耗时，在低频任务中执行）
        tmp[0] = (u16)(Q * 1000) >> 8;
        tmp[1] = (u16)(Q * 1000);
        EEPROM_write_n(0, tmp, sizeof(tmp));
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

static EC_Range_t ec_range = EC_RANGE_20MS;
static u16 range0_cnt = 0;
static u16 range5_cnt = 0;

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

static void Set_EC_Range(EC_Range_t range)
{
    ec_range = range;

    switch (range)
    {
    case EC_RANGE_20MS:

        COM_NO2;
        res_fb = 0.82f;
        DIS_LED_Just_One_Enable(1);
        break;

    case EC_RANGE_2MS:

        COM_NO1;
        res_fb = 8.2f;
        DIS_LED_Just_One_Enable(2);
        break;

    case EC_RANGE_0P2MS:

        COM_NO0;
        res_fb = 82.0f;
        DIS_LED_Just_One_Enable(3);
        break;
    }

    range0_cnt = 0;
    range5_cnt = 0;

    /* 重新从最小偏置开始 */
    pfCurrentState = Range_0;
    eCurrentEvent = EVT_NO_EVENT;

    REF_R0;
    offset_vol = 0.0f;

    delay_ms(1);
}

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
static void EC_Range_Manager(void)
{
    switch (ec_range)
    {
    case EC_RANGE_20MS:

        if (pfCurrentState == Range_0)
        {
            range0_cnt++;

            if (range0_cnt >= RANGE_SWITCH_CNT)
            {
                Set_EC_Range(EC_RANGE_2MS);
            }
        }
        else
        {
            range0_cnt = 0;
        }

        break;

    case EC_RANGE_2MS:

        if (pfCurrentState == Range_0)
        {
            range0_cnt++;

            if (range0_cnt >= RANGE_SWITCH_CNT)
            {
                Set_EC_Range(EC_RANGE_0P2MS);
            }
        }
        else
        {
            range0_cnt = 0;
        }

        if (pfCurrentState == Range_5)
        {
            range5_cnt++;

            if (range5_cnt >= RANGE_SWITCH_CNT)
            {
                Set_EC_Range(EC_RANGE_20MS);
            }
        }
        else
        {
            range5_cnt = 0;
        }

        break;

    case EC_RANGE_0P2MS:

        if (pfCurrentState == Range_5)
        {
            range5_cnt++;

            if (range5_cnt >= RANGE_SWITCH_CNT)
            {
                Set_EC_Range(EC_RANGE_2MS);
            }
        }
        else
        {
            range5_cnt = 0;
        }

        break;
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

/*
首先0-20ms/cm测量,res = 820,com_no2
当进入0-4量程
且测量信号<1.8ms/cm，res = 8.2k,com_no1
当进入0-0.4量程
且测量信号<0.18ms/cm，res = 82k,com_no0

程序架构是0-20，0-2，0-0.2三个大量程，每个量程内划分5个模拟量程进行精细化采样（Auto_Switcher已实现）。现在需要加入三个大量程的切换逻辑。
*/