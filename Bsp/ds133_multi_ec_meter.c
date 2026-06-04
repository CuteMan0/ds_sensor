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

#define Q_20MS Q_val[0]
#define Q_2MS Q_val[1]
#define Q_0P2MS Q_val[2]

#define G_20MS G_val[0]
#define G_2MS G_val[1]
#define G_0P2MS G_val[2]

typedef struct
{
    u16 q20;
    u16 q2;
    u16 q02;
} EC_Cal_t;

EC_Cal_t cal; // 校准值

static EC_Range_t ec_range = EC_RANGE_20MS;
static u16 range0_cnt = 0;
static u16 range5_cnt = 0;
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

volatile float offset_vol = 0.0f; // 减法器偏移量
float offset_val[5] = {
    0.30f,
    0.896f,
    1.433f,
    1.896f,
    2.43f};

volatile float res_fb = 0.82f; // 反馈电阻 kohm
static float Q_val[3] =
    {
        1.039f,
        1.039f,
        1.039f}; // Q值

static float G_val[3] =
    {
        0.56f,
        0.39f,
        0.35f}; // G值

static volatile u8 calibration_pending = 0; // 校准标志
static volatile u8 led_flash = 0;           // LED闪烁标志

ADC_Handle_t adc0;
ADC_Handle_t adc1;

static void Set_EC_Range(EC_Range_t range);
static void Auto_Switcher(void);
static void EC_Range_Manager(void);
static void Scan_Key(void);

void ec_init(void)
{
    adc_init(&adc0, 0, 3.3f);
    adc_init(&adc1, 1, 3.3f);

    P1_MODE_OUT_PP(GPIO_Pin_6); // P1.6设置为推挽输出(切换量程)
    P2_MODE_OUT_PP(GPIO_Pin_4); // P2.4设置为推挽输出(切换量程)
    P2_MODE_OUT_PP(GPIO_Pin_5); // P2.5设置为推挽输出(切换量程)

    P4_MODE_OUT_PP(GPIO_Pin_1); // P4.1设置为推挽输出(设置采样电阻)
    P4_MODE_OUT_PP(GPIO_Pin_0); // P4.0设置为推挽输出(设置采样电阻)

    REF_R0; // 设置基准为GND
    Set_EC_Range(EC_RANGE_20MS);
    EEPROM_read_n(0, (u8 *)&cal, sizeof(cal));
    if (cal.q20 != 0xFFFF)
        Q_20MS = cal.q20 / 1000.0f;

    if (cal.q2 != 0xFFFF)
        Q_2MS = cal.q2 / 1000.0f;

    if (cal.q02 != 0xFFFF)
        Q_0P2MS = cal.q02 / 1000.0f;
}

void ec_read(float *ec_val)
{
    float adc_vol = 0.0f;
    float q, gain;
    *ec_val = 0.0f;

    if (2 == flag_key) // 长按，开始EEPROM备份
    {
        flag_key = 0;                                          // mS/cm
        adc_vol = (adc_get(&adc0) / 5.0f + offset_vol) / 2.0f; // 信号电压
        switch (ec_range)
        {
        case EC_RANGE_20MS:

            Q_20MS =
                (12.85f * res_fb * VIN * G_20MS) / adc_vol;

            break;

        case EC_RANGE_2MS:

            Q_2MS =
                (1.413f * res_fb * VIN * G_2MS) / adc_vol;

            break;

        case EC_RANGE_0P2MS:

            Q_0P2MS =
                (0.1465f * res_fb * VIN * G_0P2MS) / adc_vol;

            break;
        }

        calibration_pending = 1;
    }

    Auto_Switcher(); // 自动切换量程
    EC_Range_Manager();

    adc_vol = (adc_get(&adc0) / 5.0f + offset_vol) / 2.0f;
    q = Q_val[ec_range];                           // 根据量程选择Q值
    gain = G_val[ec_range];                        // 根据量程选择增益
    *ec_val = adc_vol * q / (res_fb * VIN * gain); // k = Q/(R*|Vin|)*Vout,Vin = Vp * 2/pi
}

void ProcessCalibration(void)
{
    EC_Cal_t cal;

    if (calibration_pending)
    {
        calibration_pending = 0;

        cal.q20 = (u16)(Q_20MS * 1000);
        cal.q2 = (u16)(Q_2MS * 1000);
        cal.q02 = (u16)(Q_0P2MS * 1000);

        EA = 0;
        EEPROM_write_n(0, (u8 *)&cal, sizeof(cal));
        EA = 1;

        led_flash = 1;
    }
}

void EC_Led_Task(void)
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
            switch (ec_range)
            {
            case EC_RANGE_20MS:
                DIS_LED_Just_One_Enable(1);
                break;

            case EC_RANGE_2MS:
                DIS_LED_Just_One_Enable(2);
                break;

            case EC_RANGE_0P2MS:
                DIS_LED_Just_One_Enable(3);
                break;
            }
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

            switch (ec_range)
            {
            case EC_RANGE_20MS:
                DIS_LED_Just_One_Enable(1);
                break;

            case EC_RANGE_2MS:
                DIS_LED_Just_One_Enable(2);
                break;

            case EC_RANGE_0P2MS:
                DIS_LED_Just_One_Enable(3);
                break;
            }
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