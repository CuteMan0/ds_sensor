#include "ds131_ph_meter.h"

#if DS_SENSOR == 131

#include "adc_drive.h"

/* ============================================================
 * 宏定义 — pH 校准参数
 * ============================================================ */

/** @brief pH 探头零位偏移电压（V）
 *  pH 探头在中性溶液（pH≈7）附近输出约 1.25V，
 *  后续计算以该电压为基准零点。 */
#define V_OFFSET 1.25f

/** @brief 正向区域线性校准斜率（tmp > 0，即 ADC 电压高于零位偏移时） */
#define K_1 -11.4856f
/** @brief 正向区域线性校准截距 */
#define B_1 6.951f

/** @brief 负向区域线性校准斜率（tmp ≤ 0，即 ADC 电压低于零位偏移时） */
#define K_2 -12.0996f
/** @brief 负向区域线性校准截距 */
#define B_2 6.956f

/** @brief 微调偏置（mV），用于出厂校准精调 */
#define OFFSET 0.00f

/* ============================================================
 * 全局变量
 * ============================================================ */

/** @brief ADC 句柄：通道 0，参考电压 3.3V，用于采集 pH 探头电压 */
ADC_Handle_t adc0;

/** @brief pH 中间计算值（分段线性校准后的粗算结果） */
float ph_vol = 0.0f;

/* ============================================================
 * 函数实现
 * ============================================================ */

/**
 * @brief   传感器初始化
 * @note    初始化 ADC 通道 0，设置参考电压为 3.3V。
 *          该函数在 main() 启动时由 task_scheduler 调用一次。
 */
void ds_init(void)
{
    adc_init(&adc0, 0, 3.3f);
}

/**
 * @brief   pH 数据采集与处理
 * @note    处理流程：
 *          1. 通过 ADC 读取 pH 探头电压
 *          2. 减去零位偏移 V_OFFSET（1.25V），得到相对电压 tmp
 *          3. 分段线性模型计算 pH：
 *             - tmp > 0 时：pH = K_1 × (tmp - OFFSET) + B_1
 *             - tmp ≤ 0 时：pH = K_2 × (tmp - OFFSET) + B_2
 *          4. 限幅到有效范围 [0, 14]
 *          5. 送入滑动平均滤波器平滑输出
 *          6. 延时 50ms 等待下次调度
 *
 *          该函数由 task_sensor 任务周期性调用。
 */
void ds_update(void)
{

    float tmp = 0.0f;

    /* ---- 第1步：ADC 采样，减去零位偏移 ---- */
    tmp = adc_get(&adc0) - V_OFFSET;

    /* ---- 第2步：分段线性校准 ---- */
    if (tmp > 0)
    {
        /* 正向区：电压高于零位偏移，使用 K_1 / B_1 参数 */
        ph_vol = K_1 * (tmp - OFFSET) + B_1;
    }
    else
    {
        /* 负向区：电压低于零位偏移，使用 K_2 / B_2 参数 */
        ph_vol = K_2 * (tmp - OFFSET) + B_2;
    }

    /* ---- 第3步：限幅到 pH 有效范围 [0, 14] ---- */
    dat_for_printf = ph_vol < 0.0f ? 0.0f : (ph_vol > 14.0f ? 14.0f : ph_vol);

    /* ---- 第4步：滑动平均滤波 + 延时 ---- */
    avg_filter_update(&filter, dat_for_printf);
    task_delay_ms(50);
}

/**
 * @brief   格式化输出 pH 值
 * @note    通过 printf 输出 "ph:%.2f"，由 task_printf 任务周期性调用。
 */
void ds_printf(void)
{
    printf("ph:%.2f\r\n", dat_for_printf);
}

/**
 * @brief   校准保存
 * @note    当前预留，待实现 pH 校准数据的 EEPROM 写入逻辑。
 *          由 task_calibration_save 任务在触发校准后调用。
 */
void ds_calib(void)
{
    /* 待实现：pH 校准数据写入 EEPROM */
}

#endif