#include "ds161_co_meter.h"

#if DS_SENSOR == 161

#include "adc_drive.h"
#include "led_drive.h"

#define HEAT_INTERVAL (10 * 60) // 脱附间隔10分钟
#define HEAT_TIME 45            // 加热脱附45s

sbit sw = P2 ^ 4;
volatile u8 heating_flag = 0;
volatile u32 sys_tick = 0;
u32 last_heating_time = 0; // 上次加热时间
ADC_Handle_t adc0;

static void Timer3_Init(void);       // 10毫秒@24.000MHz
static void CO_HeatingControl(void); // CO加热脱附控制

void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
    sw = 0; // PMOS导通，5V加热待机状态
    Timer3_Init();
}

void ds_update(float *dat)
{
    float adc_vol = 0.0f;
    static float co_val = 0.0f;

    CO_HeatingControl();

    if (!heating_flag)
    {
        DIS_LED_Just_One_Enable(3); // 运行指示灯（未加热）

        // 未加热时才能测量
        adc_vol = adc_get(&adc0);

        if (adc_vol < 1.8f)
            co_val = 0.0f;
        else if (adc_vol < 4.1f) // 0~200
            co_val = 86.95f * adc_vol - 156.5f;
        else if (adc_vol < 4.4f) // 200~300
            co_val = 333.3f * adc_vol - 1166.6f;
        else // 300~1000
            co_val = 1521.7f * adc_vol - 6395.6f;

        *dat = (co_val > 0.1f) ? co_val : 0.0f;
    }
    else
    {
        DIS_LED_Just_One_Enable(1); // 加热指示灯
        *dat = co_val;              // 加热期间返回上次有效值
    }
}

static void CO_HeatingControl(void)
{
    u32 current_time; // 获取系统时间的函数
    current_time = sys_tick;

    if ((current_time - last_heating_time) >= HEAT_INTERVAL)
    {
        // 开始脱附加热
        if (!heating_flag)
        {
            sw = 0; // 切换至5V加热测量状态
            heating_flag = 1;
            last_heating_time = current_time;
        }
    }
    else if (heating_flag && (current_time - last_heating_time >= HEAT_TIME))
    {
        sw = 1; // 脱附加热完毕，停止加热
        heating_flag = 0;
    }
}

static void Timer3_Init(void) // 10毫秒@24.000MHz
{
    T4T3M &= 0xFD; // 定时器时钟12T模式
    T3L = 0xE0;    // 设置定时初始值
    T3H = 0xB1;    // 设置定时初始值
    T4T3M |= 0x08; // 定时器3开始计时
    IE2 |= 0x20;   // 使能定时器3中断
}

#endif
