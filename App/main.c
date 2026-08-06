// working @ 24 MHz
#define USB_INFO 1
#include "stc32g.h"
//------------------------------STC lib--------------------------
#if USB_INFO
#include "stc32_stc8_usb.h"
#endif
//------------------------------bsp lib------------------------------
#include "led_drive.h"
#include "key_drive.h"
//------------------------------  Task  ------------------------------
#include "ds_sensor.h"
#include "filter.h"
#include "task_scheduler.h"

#define NUM_BUF_AVG 8
avg_filter_t filter;
avgf_data_t buffer[NUM_BUF_AVG];
volatile float dat_for_printf;

#if USB_INFO
#include "STC32G_GPIO.h"

char putchar(char c)
{
    USB_SendData(&c, 1);
    return c;
}

void USB_init(void)
{
    P3_MODE_IN_HIZ(GPIO_Pin_0 | GPIO_Pin_1) // 需要将 P3.0/P3.1设置为高阻输入模式
    IRC48MCR = 0x80;                        // 使能内部 48M 的 USB 专用 IRC
    while (!(IRC48MCR & 0x01));
    USBCLK = 0x00; // 设置 USB 时钟源为内部 48M 的 USB 专用 IRC
    USBCON = 0x90; // 使能 USB 功能

    usb_init(); // 调用 USB CDC 初始化库函数

    EUSB = 1; // 使能 USB 中断
    EA = 1;
    while (DeviceState != DEVSTATE_CONFIGURED);
    printf("USB init OK!\n");
}
#endif

void task_calibration_save(void)
{
#if USE_CALIBRATION
    ProcessCalibration();
#endif
}

void task_key_scan()
{
    Scan_Key();
}

void task_led_blink(void)
{
#if USE_BLINK
    Led_Task();
#endif
}
void task_sensor(void)
{
    ds_update(&dat_for_printf); // 传感器数据更新
    avg_filter_update(&filter, dat_for_printf);
}

void task_printf(void)
{
#if USB_INFO
#if DS_SENSOR == 112
    printf("temp:%.2fC\n", dat_for_printf); // ℃
#elif DS_SENSOR == 131
    printf("ph:%.2f\n", dat_for_printf);
#elif DS_SENSOR == 132
    printf("EC:%.4f\n", dat_for_printf); // mS/cm
#elif DS_SENSOR == 133
    printf("EC:%.4f\n", dat_for_printf); // mS/cm
// DS134 单独设置程序文件
#elif DS_SENSOR == 135
    printf("tur:%.6f\n", dat_for_printf); // NTU
#elif DS_SENSOR == 136
    printf("ORG:%.1f\n", dat_for_printf); // mV
#elif DS_SENSOR == 137
    printf("SO2:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 138
    printf("airC2H6O:%.1f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 139
    printf("O2:%.2f%\n", dat_for_printf); // %
#elif DS_SENSOR == 141
    printf("SIG:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 144
    printf("dO:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 145
    printf("dCO2:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 151
    printf("temp:%.2fC\n", dat_for_printf); // ℃
#elif DS_SENSOR == 153
    printf("H2:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 157
    printf("BPM:%u\n", (u16)dat_for_printf); // bpm
#elif DS_SENSOR == 159
    printf("BPM:%u\n", (u16)dat_for_printf); // bpm
#elif DS_SENSOR == 160
    printf("NO2:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 161
    printf("CO:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 162
    printf("CH4:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 163
    printf("NH3:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 164
    printf("CL2:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 165
    printf("NH4:%.3f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 166
    printf("K:%.3f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 167
    printf("NO3:%.3f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 168
    printf("CL:%.3f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 169
    printf("Na:%.3f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 170
    printf("Ca:%.3f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 172
    printf("Counter:%.3f\n", dat_for_printf); // ppm
#endif
#endif
}

void STC_init(void)
{
    WTST = 0;
    EAXFR = 1;
    CKCON = 0;

#if USB_INFO
    USB_init();
#endif
    DIS_LED_init();
    KEY_GPIO_init();
}

void main(void)
{
    STC_init();
    ds_init(); // 传感器初始化
    avg_filter_init(&filter, buffer, NUM_BUF_AVG);
    task_scheduler_init();

#if DS_SENSOR == 159
    task_register(task_sensor, 80, 3);
#else
    task_register(task_sensor, 50, 3); // 传感器任务，周期50ms，优先级3
#endif
    task_register(task_printf, 100, 4);
    task_register(task_calibration_save, 1000, 1);
    task_register(task_key_scan, 20, 2);
    task_register(task_led_blink, 10, 2);

    while (1)
    {
        task_scheduler_run();
    }
}

#if DS_SENSOR == 161
#include "ds161_co_meter.h"
void Timer3_ISR_Handler(void) interrupt TMR3_VECTOR // 进中断时已经清除标志
{

    static u16 tick = 0;
    tick++;
    if (tick == 100)
    {
        tick = 0;
        heating_tick++;
    }
}
#endif

void Timer0_ISR_Handler(void) interrupt TMR0_VECTOR
{
    task_scheduler_tick_isr(); // 用于任务框架的时基
}
