/**
 * @file    ds627_MECons_drive.c
 * @brief   DS627 机械能守恒演示器 驱动层实现
 * @details 本文件实现了机械能守恒演示器的核心驱动功能，包括：
 *          - 红外光电门传感器信号采集（外部中断 + 定时器测速）
 *          - 旋臂经过各角度位置时的瞬时速度计算
 *          - 各测量点的动能、势能及总机械能计算
 *          - 通过USB串口输出测量数据
 * @note    硬件平台：STC32G 系列单片机
 */

#include "DS627_MECons_drive.h"

#include "math.h"
#include <string.h>

#include "STC32G_Delay.h"
#include "STC32G_GPIO.h"
#include "STC32G_Exti.h"
#include "STC32G_NVIC.h"
#include "STC32G_Timer.h"
#include "stc32_stc8_usb.h"

/*===========================================================================
 * 物理常量与实验参数宏定义
 *===========================================================================*/
#define LENGTH 0.27         // m  旋臂转动半径（质心到转轴距离）
#define NUMS 5              // 测量点数量（挡片个数）
#define RELEASE_ANGLE 60.0f // °  旋臂释放时的初始角度
#define M 0.03f             // kg 旋臂整体等效质量
#define G 9.80665f          // m/s^2 重力加速度标准值
#define PI 3.1415926f       // 圆周率

/*===========================================================================
 * 全局数据数组
 *===========================================================================*/

/** @brief 各测量点的瞬时速度 (m/s) */
float speed[NUMS] = {0};

/** @brief 各测量点的总机械能 (J)，Ek + Ep */
float energy_dynamic[NUMS] = {0};

/** @brief 各测量点对应的角度 (°) */
float theta[NUMS] =
    {
        //    50,
        //    45,
        //    40,
        33, /**< 第4个挡片位置角度 */
        25, /**< 第5个挡片位置角度 */
        16, /**< 第6个挡片位置角度 */
        10, /**< 第7个挡片位置角度 */
        0   /**< 第8个挡片位置角度（最低点） */
};

/*===========================================================================
 * 数据结构定义
 *===========================================================================*/

/**
 * @brief 机械能守恒实验数据结构体
 * @details 将速度、动能/总能、角度三个数组的指针封装在一起，
 *          便于统一管理和传递实验数据。
 */
typedef struct
{
    float *pspeed;      /**< 指向各测量点速度数组的指针 */
    float *penergy_dyn; /**< 指向各测量点总机械能数组的指针 */
    float *ptheta;      /**< 指向各测量点角度数组的指针 */
} mecons_t;

/** @brief 机械能守恒实验数据全局实例 */
mecons_t me_h;

/*===========================================================================
 * 硬件引脚定义
 *===========================================================================*/

/**
 * @brief 红外对射管使能引脚
 * @note  P2^0 = 1 时红外发射管发光;
 *        P2^0 = 0 时被遮挡，触发外部中断 (下降沿)
 */
sbit IR_EN = P2 ^ 0;

/*===========================================================================
 * 底层通信函数
 *===========================================================================*/

/**
 * @brief   printf 重定向字符输出函数
 * @param   c 要发送的单个字符
 * @return  返回发送的字符
 * @note    将标准输出重定向到 USB CDC 虚拟串口
 */
char putchar(char c)
{
    USB_SendData(&c, 1);
    return c;
}

/*===========================================================================
 * 硬件初始化函数
 *===========================================================================*/

/**
 * @brief  配置外部中断 INT0 (P3.2)
 * @note   使用上升沿和下降沿均触发模式 (EXT_MODE_RiseFall),
 *         用于检测红外光电门信号的变化;
 *         中断优先级为 Priority_1
 */
static void Exti_config(void)
{
    EXTI_InitTypeDef Exti_InitStructure;

    Exti_InitStructure.EXTI_Mode = EXT_MODE_RiseFall;
    Ext_Inilize(EXT_INT0, &Exti_InitStructure);
    NVIC_INT0_Init(ENABLE, Priority_1);
}

/**
 * @brief  配置定时器 Timer0
 * @note   模式: 16位自动重载;
 *         时钟源: 12T 分频（系统时钟/12）;
 *         分频器 PS=19, 系统主频假定 24MHz 时，
 *         计数周期 = 12/(24M) * (19+1) = 10us，即每 0.01ms 计数一次;
 *         初始状态为关闭，测速时由中断服务程序启停
 */
static void Timer_config(void)
{
    TIM_InitTypeDef TIM_InitStructure;

    TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;
    TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;
    TIM_InitStructure.TIM_ClkOut = DISABLE;
    TIM_InitStructure.TIM_Value = 0;
    TIM_InitStructure.TIM_PS = 19;
    TIM_InitStructure.TIM_Run = DISABLE;
    Timer_Inilize(Timer0, &TIM_InitStructure);
}

/**
 * @brief  机械能守恒实验模块初始化
 * @details 依次完成以下初始化工作：
 *          1. 使能红外发射管 (IR_EN = 1)
 *          2. 配置 P2.0 为推挽输出，P3.2 为带上拉输入
 *          3. 延时 100ms 等待外设稳定
 *          4. 配置外部中断 INT0
 *          5. 配置定时器 Timer0
 *          6. 将全局数据数组绑定到 me_h 结构体实例
 */
void ME_Init(void)
{
    /* 红外发射管使能，高电平发光 */
    IR_EN = 1;
    /* 配置 P2.0 为推挽输出模式，驱动红外发射管 */
    P2_MODE_OUT_PP(GPIO_Pin_0);
    /* 配置 P3.2 (INT0) 为带上拉输入模式，接收红外接收管信号 */
    P3_MODE_IO_PU(GPIO_Pin_2);
    P3_PULL_UP_ENABLE(GPIO_Pin_2);
    /* 延时等待外设稳定 */
    delay_ms(100);
    /* 配置外部中断 */
    Exti_config();
    /* 配置定时器 */
    Timer_config();

    /* 将全局数据数组的地址赋给结构体指针成员 */
    me_h.penergy_dyn = energy_dynamic;
    me_h.pspeed = speed;
    me_h.ptheta = theta;
}

/*===========================================================================
 * 核心数据处理函数
 *===========================================================================*/

/**
 * @brief  处理光电门测速数据并计算各点机械能
 * @details 当外部中断 ISR 中设置 data_unlocked 标志后, 主循环调用本函数:
 *          1. 根据计数值 tick_count 计算瞬时速度
 *          2. 遍历所有已记录的测量点
 *          3. 计算各点的动能 Ek = 0.5 * m * v^2
 *          4. 计算各点的势能 Ep = m * g * r * (cosθ? - cosθ)
 *             其中 θ? = 64° 为释放初始角度
 *          5. 计算总机械能 E = Ek + Ep，验证机械能守恒
 *          6. 通过 printf 输出各测量点的角度、速度、动能、势能、总机械能
 * @note   数据就绪标志 data_unlocked 在外部中断 ISR 中置位，
 *         本函数处理完毕后清除该标志。
 * @note   能量计算假设零势能面在转轴水平面上，
 *         即最低点 (θ=0°) 处势能最低。
 */
void ME_GetSpeed(void)
{
    /** @brief 当前写入位置的环形缓冲区索引 */
    static u8 pos = 0;
    u8 i;

    /* 检查是否有新的测速数据就绪 */
    if (data_unlcoked)
    {
        /* 计数值为 0 表示挡片通过时间极短或异常，跳过本次处理 */
        if (tick_count == 0)
        {
            data_unlcoked = 0;
            return;
        }

        /*------------------------------------------------------------------
         * 瞬时速度计算
         *  挡片宽度: 5mm = 0.005m
         *  tick_count 单位: 0.01ms (10us)
         *  通过时间 t(s) = tick_count * 0.01ms = tick_count * 1e-5 s
         *  瞬时速度 v = 挡片宽度 / 通过时间
         *              = 0.005 / (tick_count * 1e-5)
         *              = 500 / tick_count  (m/s)
         *------------------------------------------------------------------*/
        *(me_h.pspeed + pos) = 500 / (float)tick_count;

        /* 输出本次计数值对应的挡片通过时间 (单位: ms) */
        printf("count=%.4f\r\n", tick_count * 0.01);

        /* 环形缓冲区索引递增，循环覆盖旧数据 */
        pos = (pos + 1) % NUMS;

        /*------------------------------------------------------------------
         * 遍历所有测量点，计算动能、势能、总机械能
         *------------------------------------------------------------------*/
        for (i = 0; i < NUMS; i++)
        {
            /**
             * 动能计算: Ek = 0.5 * m * v^2
             * @note 旋臂为刚体，表达式采用 m*v^2 简化模型，
             *       将转动动能等效为质心平动动能
             */
            float kinetic = 0.5f * M * me_h.pspeed[i] * me_h.pspeed[i];

            /** 角度转弧度: θ_rad = θ_deg * π / 180 */
            float theta_rad = me_h.ptheta[i] * PI / 180.0f;

            /**
             * 势能计算（以转轴水平面为参考零势能面）
             * Ep = m * g * h
             * 高度变化: Δh = r * (cosθ? - cosθ)
             * 其中:
             *   θ? = 64° 释放初始角度 (最高点)
             *   θ   = 当前测量点角度
             *   当 θ < θ? 时，旋臂下降，cosθ > cosθ?，Ep 为正，合理
             */
            float potential = M * G * LENGTH *
                              (cos(RELEASE_ANGLE * PI / 180.0f) - cos(theta_rad));

            /** 总机械能: E = Ek + Ep (理论上应保持不变，验证守恒) */
            me_h.penergy_dyn[i] = kinetic + potential;

            /* 格式化输出各测量点数据 */
            printf("%2d  %2.0fdeg  ", i + 1, me_h.ptheta[i]); /* 序号 & 角度 */
            printf("v=%6.3f ", me_h.pspeed[i]);               /* 瞬时速度 */
            printf("Ek=%7.4f ", kinetic);                     /* 动能 Ek */
            printf("Ep=%7.4f ", potential);                   /* 势能 Ep */
            printf("E=%7.4f\r\n", me_h.penergy_dyn[i]);       /* 总机械能 E */
        }

        /* 数据处理完成，清除数据就绪标志 */
        data_unlcoked = 0;
    }
}
