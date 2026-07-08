#include "key_handler.h"

#include "KEY_drive.h"
#include "multi_button.h"

#include <stdio.h>
#include <stdlib.h>

#include "log.h"

static Button home;

uint8_t read_button_gpio(uint8_t button_id)
{
    u8 t;
    switch (button_id) {
        case 1:
            return gpio_ReadPin(3,2);
        case 2:
            return ;
        default:
            return 0;
    }
}

void btn1_single_click_handler(Button* btn)
{
    (void)btn;  // suppress unused parameter warning
    printf("Button 1: Single Click\n");
}

void btn1_double_click_handler(Button* btn)
{
    (void)btn;  // suppress unused parameter warning
    printf("??Button 1: Double Click\n");
}

void btn1_long_press_start_handler(Button* btn)
{
    (void)btn;  // suppress unused parameter warning
    printf("Button 1: Long Press Start\n");
}

void btn1_long_press_hold_handler(Button* btn)
{
    (void)btn;  // suppress unused parameter warning
    printf("Button 1: Long Press Hold...\n");
}

void btn1_press_repeat_handler(Button* btn)
{
    printf("Button 1: Press Repeat (count: %d)\n", button_get_repeat_count(btn));
}

void key_init(void)
{
    gpio_init();
    // 初始化按键 (active_level: 0=低电平有效, 1=高电平有效)
    button_init(&home, read_button_gpio, 0, 1);
    
    button_attach(&home, BTN_SINGLE_CLICK, btn1_single_click_handler);
    button_attach(&home, BTN_DOUBLE_CLICK, btn1_double_click_handler);
    button_attach(&home, BTN_LONG_PRESS_START, btn1_long_press_start_handler);
    button_attach(&home, BTN_LONG_PRESS_HOLD, btn1_long_press_hold_handler);
    button_attach(&home, BTN_PRESS_REPEAT, btn1_press_repeat_handler);

    button_start(&home);
}

void key_scan(void)
{
    button_ticks(); // 每10ms调用一次
}
