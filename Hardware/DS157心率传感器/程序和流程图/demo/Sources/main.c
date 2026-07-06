//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: main.c
// 文件描述: 
// 文件版本: V1.0
// 修改记录:
//   1. (2026-07-06) 创建文件
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#include "config.h"                     //默认已包含stdio.h、intrins.h等头文件


//<<AICUBE_USER_INCLUDE_BEGIN>>
// 在此添加用户头文件包含  
#include "max30102.h"
#include "fft.h"
//<<AICUBE_USER_INCLUDE_END>>


//<<AICUBE_USER_GLOBAL_DEFINE_BEGIN>>
// 在此添加用户全局变量定义、用户宏定义以及函数声明
void task_update(void);
void task_sensor(void);
float FFT_FreDetect(float *raw, float sample_rate);

#define PI 3.1415926f
#define FS 100.0f
#define N MAX_FFT_N
float xdata fifo_buf[MAX_FFT_N];
complex_t xdata fft_out[MAX_FFT_N];
float xdata mag[MAX_FFT_N];

uint32_t red_fifo = 0;
uint32_t ir_fifo = 0;

u8 data_ready;

u16 fill = 0;
u16 k;
//<<AICUBE_USER_GLOBAL_DEFINE_END>>



////////////////////////////////////////
// 项目主函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void main(void)
{
    //<<AICUBE_USER_MAIN_INITIAL_BEGIN>>
    // 在此添加用户主函数初始化代码  
    //<<AICUBE_USER_MAIN_INITIAL_END>>

    SYS_Init();

    //<<AICUBE_USER_MAIN_CODE_BEGIN>>
    // 在此添加主函数中运行一次的用户代码  
    fft_init();
    max30102_init();
    //<<AICUBE_USER_MAIN_CODE_END>>

    while (1)
    {
        //<<AICUBE_USER_MAIN_LOOP_BEGIN>>
        // 在此添加主函数中用户主循环代码  
        task_update();
        task_sensor();
        delay_ms(10);
        //<<AICUBE_USER_MAIN_LOOP_END>>
    }
}

////////////////////////////////////////
// 系统初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void SYS_Init(void)
{
    EnableAccessXFR();                  //使能访问扩展XFR
    AccessCodeFastest();                //设置最快速度访问程序代码
    AccessIXramSpeed(1);                //设置访问内部XDATA的速度（如果不使能DMA，可将参数设置为0）
    IAP_SetTimeBase();                  //设置IAP等待参数,产生1us时基

    //<<AICUBE_USER_PREINITIAL_CODE_BEGIN>>
    // 在此添加用户预初始化代码  
    //<<AICUBE_USER_PREINITIAL_CODE_END>>

    PORT1_Init();                       //P1口初始化
    PORT2_Init();                       //P2口初始化
    PORT3_Init();                       //P3口初始化
    CLK_Init();                         //时钟模块初始化
    delay_ms(1);
    USBLIB_Init();                      //USB库初始化
    delay_ms(1);

    //<<AICUBE_USER_INITIAL_CODE_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_INITIAL_CODE_END>>

    EnableGlobalInt();                  //使能全局中断
    USBLIB_WaitConfiged();              //等待USB完成配置
}

////////////////////////////////////////
// 微秒延时函数
// 入口参数: us (设置延时的微秒值)
// 函数返回: 无
////////////////////////////////////////
void delay_us(uint16_t us)
{
    do
    {
        NOP(34);                        //(MAIN_Fosc + 500000) / 1000000 - 6
    } while (--us);
}


////////////////////////////////////////
// 毫秒延时函数
// 入口参数: ms (设置延时的毫秒值)
// 函数返回: 无
////////////////////////////////////////
void delay_ms(uint16_t ms)
{
    uint16_t i;

    do
    {
        i = MAIN_Fosc / 6000;
        while (--i);
    } while (--ms);
}


//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码 
void task_sensor(void)
{
    u16 i;
    float f;
    float heart_hz,heart_bpm = 0;

    if(!data_ready)
        return;
    data_ready = 0;

    heart_hz  = FFT_FreDetect(fifo_buf, FS);
    heart_bpm = heart_hz * 60.0f;

    printf("Heart: %.1f BPM\r\n",heart_bpm);

}

void task_update(void)
{
        max30102_readFIFO(&red_fifo,&ir_fifo);

        //填充FFT数组
        if (fill < N)
        {
            fifo_buf[fill] = red_fifo;
            fill++;

            if (N == fill)
                data_ready = 1;
        }
        else
        {
            for (k = 0; k < N - 1; k++)
                fifo_buf[k] = fifo_buf[k + 1];
            fifo_buf[N - 1] = red_fifo;
            
            data_ready = 1;
        }
}

float FFT_FreDetect(float *raw, float sample_rate)
{
#define THRESHOLD 100.0f
    u16 i, max_bin;
    float dc, freq, max_val, min_freq, max_freq, f;
    float xdata tmp[MAX_FFT_N];
    
    /* BPF */
    min_freq = 0.66f;//  40BPM
    max_freq = 4.0f; // 240BPM

    //去直流
    dc = 0;
    for (i = 0; i < MAX_FFT_N; i++)
        dc += raw[i];
    dc /= (float)MAX_FFT_N;
    
    for (i = 0; i < MAX_FFT_N; i++)
        tmp[i] = raw[i] - dc;

    /* Hann */
    for (i = 0; i < MAX_FFT_N; i++)
        tmp[i] = tmp[i] * (0.5f - 0.5f * cos(2 * PI * i / (MAX_FFT_N - 1)));

    /* FFT */
    fft_run(tmp, fft_out);
    fft_abs(fft_out, mag);

    /* 频率峰值 */
    max_val = 0;
    max_bin = 0;
    for (i = 1; i < MAX_FFT_N / 2; i++)
    {
        f = (float)i * sample_rate / (float)MAX_FFT_N;
        if (f >= min_freq && f <= max_freq)
        {
            if (mag[i] > max_val)
            {
                max_val = mag[i];
                max_bin = i;
            }
        }
    }

    if (0 == max_bin || THRESHOLD > max_val)
        return 0.0f;

    freq = (float)max_bin * sample_rate / (float)MAX_FFT_N;
    return freq;
}
//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


