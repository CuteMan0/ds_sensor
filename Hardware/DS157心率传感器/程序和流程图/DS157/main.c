// 测试工作频率为 24 MHz
#include "stc32g.h"

#include <stdio.h>

#include "LED_drive.h"
#include "USB_CDC_drive.h"
#include "ADC_drive.h"
#include	"STC32G_Delay.h"

#include "log.h"

#define VOL_TH 1.5f // 高电平
#define VOL_TL 0.3f // 低电平


ADC_Handle_t adc0;

// 全局变量定义
#define FILTER_SIZE 5
static u16 bpm_buffer[FILTER_SIZE];  // 存储5个BPM值
static u8 buffer_index = 0;
static u8 buffer_filled = 0;          // 是否已填满缓冲区

u8 i;
// ====== 全局标志位 ======
volatile u8 first_trigger = 0;      
volatile u8 second_trigger = 0;     
volatile u8 detect_ready = 0;
volatile u16 first_tick = 0;        
volatile u16 second_tick = 0;       
volatile u16 delta_tick = 0;        
volatile u16 tick_counter = 0;      

void add_bpm_value(u16 bpm_value);
u16 get_filtered_bpm(void);

void main(void)
{
    WTST = 0;
    EAXFR = 1;
    CKCON = 0;

    USB_CDC_Initialization();
    EA = 1;
    while (DeviceState != DEVSTATE_CONFIGURED);
    
    led_init();
    adc_init(&adc0,0,3.3);

    // 1ms@24.000MHz
    TM0PS = 0x00; // Set timer clock prescaler
    AUXR |= 0x80; // imer clock is 1T mode
    TMOD &= 0xF0; // Set timer work mode
    TL0 = 0x40;   // Initial timer value
    TH0 = 0xA2;   // Initial timer value
    TF0 = 0;      // Clear TF0 flag
    TR0 = 1;      // Timer0 start run
    ET0 = 1;      // Enable timer0 interrupt
    
    for ( i = 0; i < FILTER_SIZE; i++)
    {
        bpm_buffer[i] = 0;
    }
    buffer_index = 0;
    buffer_filled = 0;
    
    while (1)
    {
        if (detect_ready)  // 检测完成才处理
        {
            u16 current_bpm = 60000 / delta_tick;
            u16 filtered_bpm;
            
            // 有效性检查：合理的BPM范围（30-200）
            if (current_bpm >= 30 && current_bpm <= 200)
            {
                add_bpm_value(current_bpm);
                
                // 打印滤波后的结果
                filtered_bpm = get_filtered_bpm();
                printf("BPM: %u\r\n", filtered_bpm);
            }
            else
            {
                // 无效数据，丢弃
//                printf("Invalid BPM: %u\r\n", current_bpm);
            }
            
            detect_ready = 0;  // 清除标志
        }
    }
}

void Timer0_Isr(void) interrupt 1
{
    static u8 last_state, current_state = 0;
    float adc_tmp;
    
    tick_counter++;  // tick计数器递增
    
    // 读取ADC（确保adc_get函数不依赖中断）
    adc_tmp = adc_get(&adc0);
    
    // 判断当前是否超过阈值
    current_state = (adc_tmp > VOL_TH) ? 1 : 0;
    
    // 检测上升沿（0->1 变化）
    if (current_state == 1 && last_state == 0)
    {
        if (!first_trigger)  // 第一次上升沿
        {
            first_trigger = 1;
            first_tick = tick_counter;
        }
        else if (!second_trigger)  // 第二次上升沿
        {
            second_trigger = 1;
            second_tick = tick_counter;
            
            // 计算时间差
            delta_tick = second_tick - first_tick;
            
            // 设置检测完成标志
            detect_ready = 1;
            
            // 重置状态，准备下一次检测
            first_trigger = 0;
            second_trigger = 0;
        }
    }
    // 更新上次状态
    last_state = current_state;
}

// 排序函数（冒泡排序，从小到大）
void sort_array(u16 arr[], u8 len)
{
    u8 i, j;
    u16 temp;
    for (i = 0; i < len - 1; i++)
    {
        for (j = 0; j < len - 1 - i; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// 中值滤波+均值滤波函数
u16 median_mean_filter(u16 arr[], u8 len)
{
    u8 i;
    u32 sum = 0;
    u16 sorted_arr[FILTER_SIZE];
    
    // 复制数组
    for (i = 0; i < len; i++)
    {
        sorted_arr[i] = arr[i];
    }
    
    // 排序
    sort_array(sorted_arr, len);
    
    // 去掉最大值和最小值，对中间3个值求平均
    for (i = 1; i < len - 1; i++)  // i从1到len-2
    {
        sum += sorted_arr[i];
    }
    
    return (u16)(sum / (len - 2));  // 返回均值
}

// 添加新BPM值到缓冲区
void add_bpm_value(u16 bpm_value)
{
    bpm_buffer[buffer_index] = bpm_value;
    buffer_index++;
    
    if (buffer_index >= FILTER_SIZE)
    {
        buffer_index = 0;
        buffer_filled = 1;  // 标记已填满
    }
}

// 获取滤波后的BPM值
u16 get_filtered_bpm(void)
{
    if (buffer_filled)
    {
        return median_mean_filter(bpm_buffer, FILTER_SIZE);
    }
    else if (buffer_index > 0)
    {
        // 如果缓冲区未满，对已有数据滤波（但至少需要3个数据）
        if (buffer_index >= 3)
        {
            return median_mean_filter(bpm_buffer, buffer_index);
        }
        else
        {
            // 数据太少，直接返回最新值
            return bpm_buffer[buffer_index - 1];
        }
    }
    else
    {
        return 0;  // 无数据
    }
}