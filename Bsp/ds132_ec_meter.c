#include "ds132_ec_meter.h"

#if DS_SENSOR == 132

#include "stc32g.h"
#include "STC32G_Delay.h"
#include "STC32G_EEPROM.h"
#include "STC32G_GPIO.h"

#include "adc_drive.h"
#include "key_drive.h"
#include "led_drive.h"

#define VIN 0.1415f

sbit _C = P1 ^ 6;
sbit _B = P2 ^ 5;
sbit _A = P2 ^ 4;

#define SEL(x, y, z) \
    _C = (x);        \
    _B = (y);        \
    _A = (z)

#define REF_R0 SEL(0, 0, 0) // X0    0V
#define REF_R1 SEL(1, 1, 1) // X7  0.299V
#define REF_R2 SEL(1, 1, 0) // X6 0.888V
#define REF_R3 SEL(1, 0, 0) // X4  1.418V
#define REF_R4 SEL(1, 0, 1) // X5  1.6455V 异常值
#define REF_R5 SEL(0, 1, 0) // X2  2.507V

static volatile float Q = 1.01f;   // 电导池常数
volatile float res_fb = 82.0f;    // 反馈电阻 kohm
volatile float offset_vol = 0.0f; // 减法器偏移量

u8 flag_backup = 0;
// 校准延迟处理变量
static volatile u8 calibration_pending = 0;

ADC_Handle_t adc0;
ADC_Handle_t adc1;

static void Auto_Switcher(void);
static void Scan_Key(void);

void ec_init(void)
{
    u8 tmp_Q[2];
    
    adc_init(&adc0, 0, 3.3f);
    adc_init(&adc1, 1, 3.3f);

    P1_MODE_OUT_PP(GPIO_Pin_6); // P1.6设置为推挽输出(切换量程)
    P2_MODE_OUT_PP(GPIO_Pin_4); // P2.4设置为推挽输出(切换量程)
    P2_MODE_OUT_PP(GPIO_Pin_5); // P2.5设置为推挽输出(切换量程)

    REF_R0; // 设置基准为GND
    EEPROM_read_n(0, tmp_Q, sizeof(tmp_Q));
    if (tmp[0] != 0xff && tmp[1] != 0xff)
    {
        Q = (float)((u16)(tmp_Q[0] << 8) + tmp_Q[1]) / 1000;
    }
}

void ec_read(float *ec_val)
{
    float adc_vol = 0.0f;
    *ec_val = 0.0f;
    
    if (1 == flag_key) // 开始EEPROM备份
    {
        flag_key = 0;
#define K 12.85f // mS/cm
        adc_vol = adc_get(&adc1);//获取最初处理的信号电压
        Q = (K * res_fb * VIN / adc_vol); // new Q value

        // LED指示
        DIS_LED_Just_One_Enable(2);
    }
        
    Auto_Switcher(); // 自动切换量程

    adc_vol = (adc_get(&adc0) / 5.0f + offset_vol ) / 2.0f;
    *ec_val = adc_vol * Q / (res_fb * VIN) / 0.7066f; // k = Q/(R*|Vin|)*Vout
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

    if (tmp > 806)
    {
        eCurrentEvent = EVT_OverRange;
    }
}
void Range_1(void)
{
    u16 tmp;
    REF_R1;
    offset_vol = 0.299f;
    adc_get(&adc1);
    tmp = adc1.raw;

    if (tmp > 1451)
    {
        eCurrentEvent = EVT_OverRange;
    }
    if (tmp < 645)
    {
        eCurrentEvent = EVT_UnderRange;
    }
}
void Range_2(void)
{
    u16 tmp;
    REF_R2;
    offset_vol = 0.888f;
    adc_get(&adc1);
    tmp = adc1.raw;
    
    if (tmp > 2095)
    {
        eCurrentEvent = EVT_OverRange;
    }
    if (tmp < 1289)
    {
        eCurrentEvent = EVT_UnderRange;
    }
}
void Range_3(void)
{
    u16 tmp;
    REF_R3;
    offset_vol = 1.418f;
    adc_get(&adc1);
    tmp = adc1.raw;

    if (tmp > 2740)
    {
        eCurrentEvent = EVT_OverRange;
    }
    if (tmp < 1934)
    {
        eCurrentEvent = EVT_UnderRange;
    }
}
void Range_4(void)
{
    u16 tmp;
    REF_R4;
    offset_vol = 1.655f;
    adc_get(&adc1);
    tmp = adc1.raw;
    
    if (tmp > 3385)
    {
        eCurrentEvent = EVT_OverRange;
    }
    if (tmp < 2579)
    {
        eCurrentEvent = EVT_UnderRange;
    }
}
void Range_5(void)
{
    u16 tmp;
    REF_R5;
    offset_vol = 2.507f;
    adc_get(&adc1);
    tmp = adc1.raw;

    if (tmp < 3224)
    {
        eCurrentEvent = EVT_UnderRange;
    }
}

#endif