// working @ 24 MHz
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
#include "marco_config.h"

/* ============================================================
 * 全局变量定义（在 ds_sensor.h 中 extern 声明）
 * ============================================================ */
volatile float dat_for_printf;
avg_filter_t filter;
avgf_data_t buffer[NUM_BUF_AVG];

#if USB_INFO

void CDC_init(void)
{
    P3M1 |= (0x03), P3M0 &= ~(0x03); // 需要将 P3.0/P3.1设置为高阻输入模式
    usb_init();                      // USB CDC 接口配置
    EA = 1;
    printf("USB init OK!\n");
}
char putchar(char c)
{
    USB_SendData(&c, 1);
    return c;
}

#endif

void task_calibration_save(void)
{
    ds_calib(); // 统一校准接口（无校准则空函数）
    task_delay_ms(1000);
}

void task_key_scan()
{
    Scan_Key();
    task_delay_ms(20);
}

void task_led_blink(void)
{
#if USE_BLINK
    Led_Task();
#endif
    task_delay_ms(10);
}

void task_sensor(void)
{
    ds_update(); // 统一传感器更新接口（内部含采集 + 滤波 + 延时）
}

void task_printf(void)
{
#if USB_INFO
    ds_printf(); // 统一打印接口（每个 dsXX 自己决定格式和多值输出）
#endif
    task_delay_ms(100);
}

void STC_init(void)
{
    WTST = 0;
    EAXFR = 1;
    CKCON = 0;

#if USB_INFO
    CDC_init();
#endif
    DIS_LED_init();
    KEY_GPIO_init();
}

void software_init(void)
{
    avg_filter_init(&filter, buffer, NUM_BUF_AVG);
}

void main(void)
{
    STC_init();
    ds_init(); // 传感器初始化（由 dsXX 文件实现）
    software_init();
    task_scheduler_init();

    task_register(task_sensor, 3);
    task_register(task_printf, 4);
    task_register(task_calibration_save, 1);
    task_register(task_key_scan, 2);
    task_register(task_led_blink, 2);

    while (1)
    {
        task_scheduler_run();
    }
}

void Timer0_ISR_Handler(void) interrupt TMR0_VECTOR
{
    task_scheduler_tick_isr(); // 用于任务框架的时基
}