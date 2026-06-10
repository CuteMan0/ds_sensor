// working @ 24 MHz
#include "main.h"
//------------------------------STC lib--------------------------
#if USB_INFO
#include "stc32_stc8_usb.h"
#endif
//------------------------------bsp lib------------------------------
#include "USB_CDC_drive.h"
#include "led_drive.h"
#include "key_drive.h"
//------------------------------  Task  ------------------------------
#include "ds_sensor.h"
#include "filter.h"
#include "task_scheduler.h"

#define NUM_BUF_AVG 20
avg_filter_t filter;
avgf_data_t buffer[NUM_BUF_AVG];
float dat_for_printf;

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
#elif DS_SENSOR == 135
    printf("tur:%.6f\n", dat_for_printf); // NTU
#elif DS_SENSOR == 136
    printf("ORG:%.1f\n", dat_for_printf); // mV
#elif DS_SENSOR == 137
    printf("SO2:%.1f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 138
    printf("airC2H6O:%.1f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 139
    printf("O2:%.2f%\n", dat_for_printf);
#elif DS_SENSOR == 144
    printf("dO:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 145
    printf("dCO2:%.2f\n", dat_for_printf); // ppm
#elif DS_SENSOR == 153
    printf("H2:%.2f\n", dat_for_printf); // ppm
#endif
#endif
}

void main(void)
{
    startup();

    DIS_LED_init();
    KEY_GPIO_init();

    ds_init(); // 传感器初始化
    avg_filter_init(&filter, buffer, NUM_BUF_AVG);
    task_scheduler_init();
    task_register(task_sensor, 50, 3);
    task_register(task_printf, 300, 4);
    task_register(task_calibration_save, 1000, 1);
    task_register(task_key_scan, 20, 2);
    task_register(task_led_blink, 10, 2);

    while (1)
    {
        task_scheduler_run();
    }
}

void Timer0_ISR_Handler(void) interrupt TMR0_VECTOR
{
    task_scheduler_tick_isr(); // 用于任务框架的时基
}

void startup(void)
{
    WTST = 0;
    EAXFR = 1;
    CKCON = 0;

#if USB_INFO
    USB_CDC_Initialization();
    EA = 1;
    while (DeviceState != DEVSTATE_CONFIGURED)
        ;
    printf("USB Ready!\n");
#endif
}

#if USB_INFO
char putchar(char c)
{
    USB_SendData(&c, 1);
    return c;
}
#endif