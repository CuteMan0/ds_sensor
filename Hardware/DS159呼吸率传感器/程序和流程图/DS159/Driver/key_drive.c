#include "KEY_drive.h"

#include "multi_button.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

void gpio_init(void)
{
	P3_MODE_IO_PU(GPIO_Pin_2);
	P3_PULL_UP_ENABLE(GPIO_Pin_2);
}

u8 gpio_ReadPin(u8 port, u8 pin)
{
    switch (port)
    {
        case 0: return (P0 >> pin) & 0x01;
        case 1: return (P1 >> pin) & 0x01;
        case 2: return (P2 >> pin) & 0x01;
        case 3: return (P3 >> pin) & 0x01;
        case 4: return (P4 >> pin) & 0x01;
        case 5: return (P5 >> pin) & 0x01;
        case 6: return (P6 >> pin) & 0x01;
        case 7: return (P7 >> pin) & 0x01;
        default: return 0;
    } 
}

void gpio_WritePin(u8 port, u8 pin, u8 level)
{
    switch (port)
    {
        case 0:
            if (level) P0 |= (1 << pin);
            else       P0 &= ~(1 << pin);
            break;
        case 1:
            if (level) P1 |= (1 << pin);
            else       P1 &= ~(1 << pin);
            break;
        case 2:
            if (level) P2 |= (1 << pin);
            else       P2 &= ~(1 << pin);
            break;
        case 3:
            if (level) P3 |= (1 << pin);
            else       P3 &= ~(1 << pin);
            break;
        case 4:
            if (level) P4 |= (1 << pin);
            else       P4 &= ~(1 << pin);
            break;
        case 5:
            if (level) P5 |= (1 << pin);
            else       P5 &= ~(1 << pin);
            break;
        case 6:
            if (level) P6 |= (1 << pin);
            else       P6 &= ~(1 << pin);
            break;
        case 7:
            if (level) P7 |= (1 << pin);
            else       P7 &= ~(1 << pin);
            break;
        default:
            break;
    }
}