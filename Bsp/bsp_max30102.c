#include "bsp_max30102.h"

#include "sw_iic_drive.h"

#include "stc32g.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

u8 max30102_i2c_write(u8 reg_adder, u8 dat)
{
    return soft_iic_write(MAX30102_DEV_ADDRESS, reg_adder, &dat, 1);
}

u8 max30102_i2c_read(u8 reg_adder, u8 *pdat, u8 data_size)
{
    return soft_iic_read(MAX30102_DEV_ADDRESS, reg_adder, pdat, data_size);
}

void max30102_init(void)
{
    u8 dat;
    
    max30102_i2c_write(REG_MODE_CONFIG, 0x40); // reset the device

    delay_ms(5);

    max30102_i2c_write(REG_INTR_ENABLE_1, 0xE0);
    max30102_i2c_write(REG_INTR_ENABLE_2, 0x02); // interrupt enable: FIFO almost full flag, new FIFO Data Ready,

    max30102_i2c_write(REG_FIFO_WR_PTR, 0x00);
    max30102_i2c_write(REG_OVF_COUNTER, 0x00);
    max30102_i2c_write(REG_FIFO_RD_PTR, 0x00); // clear the pointer

    max30102_i2c_write(REG_FIFO_CONFIG, 0x0F); // FIFO configuration: sample averaging(1),FIFO rolls on full(0), FIFO almost full value(15 empty data samples when interrupt is issued)

    max30102_i2c_write(REG_MODE_CONFIG, 0x03); // FIFO configuration:SpO2 mode

    max30102_i2c_write(REG_SPO2_CONFIG, 0x26); // SpO2 configuration:ACD resolution:15.63pA,sample rate control:100Hz, LED pulse width:411 us

    // LED current
    max30102_i2c_write(REG_LED1_PA, 0x00); // RED
    max30102_i2c_write(REG_LED2_PA, 0x2f); // IR

    max30102_i2c_write(REG_TEMP_CONFIG, 0x01); // temperture

    max30102_i2c_read(REG_INTR_STATUS_1, &dat, 1);
    max30102_i2c_read(REG_INTR_STATUS_2, &dat, 1); // clear the flag
}

void max30102_fifo_read(float *output_data)
{
    u8 receive_data[6] = {0};
    u8 temp_data = 0;
    u32 dat[2];

    max30102_i2c_read(REG_INTR_STATUS_1, &temp_data, 1);

    while ((temp_data & 0x40) != 0x40)
    {
        max30102_i2c_read(REG_INTR_STATUS_1, &temp_data, 1);
    }
    max30102_i2c_read(REG_FIFO_DATA, receive_data, 6);
    dat[0] = ((receive_data[0] << 16 | receive_data[1] << 8 | receive_data[2]) & 0x03ffff); // RED
    dat[1] = ((receive_data[3] << 16 | receive_data[4] << 8 | receive_data[5]) & 0x03ffff); // IR
    *output_data = dat[1];
}