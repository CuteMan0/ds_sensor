#include "key_drive.h"

#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

static u8 key_state = 0;
static u16 key_press_time = 0;
static u8 long_press_triggered = 0;
u8 flag_key = 0; // 1 = 短按，2 = 长按

//========================================================================
// 函数: void KEY_GPIO_init(void)
// 描述: 按键_IO口配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-06-26
void KEY_GPIO_init(void)
{
    P3_MODE_IN_HIZ(GPIO_Pin_2);    // P3.2设置为准双向口
    P3_PULL_UP_ENABLE(GPIO_Pin_2); // P3.2上拉使能
}

void Scan_Key(void)
{
    u8 current_key = (0 == HOME_key) ? 0 : 1;

    switch (key_state)
    {
    case 0: // 空闲
        if (0 == current_key)
        {
            key_state = 1;
            key_press_time = 0;
            long_press_triggered = 0;
        }
        break;

    case 1: // 消抖
        if (0 == current_key)
        {
            key_press_time++;
            if (key_press_time >= 2) // 消抖完成（40ms）
            {
                key_state = 2;
            }
        }
        else
        {
            key_state = 0;
        }
        break;

    case 2: // 按键按下，等待释放或长按
        if (0 == current_key)
        {
            key_press_time++;

            // 长按检测（例如 1000ms）
            if (!long_press_triggered && key_press_time >= 50) // 50 * 20ms = 1000ms
            {
                long_press_triggered = 1;
                flag_key = 2; // 长按，
            }
        }
        else // 按键释放
        {
            if (!long_press_triggered)
            {
                flag_key = 1; // 短按，校准
            }
            key_state = 0;
        }
        break;
    }
}