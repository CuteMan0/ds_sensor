/*红外温度传感器 */
#include "bsp_mlx90614.h"

#if DS_SENSOR == 112

#include "stc32g.h"
#include "sw_iic_drive.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

u8 mlx_iic_read(u8 addr, u8 reg, u8 *buf, u16 len)
{
    u8 i;
    soft_i2c_start();
    soft_i2c_write_byte(addr << 1);
    soft_i2c_write_byte(reg);
    soft_i2c_start();
    soft_i2c_write_byte(addr << 1 | 0x01);
    for (i = 0; i < len - 1; i++)
        *(buf + i) = soft_i2c_read_byte(1);
    *(buf + i) = soft_i2c_read_byte(0);
    soft_i2c_stop();
}

/**
 * @brief  初始化mlx90614红外温度传感器
 * @retval 成功返回0，失败返回1
 */
u8 mlx_init(void)
{
    /**********************************外设初始化**********************************/
    soft_i2c_init();
    /*********************************传感器初始化*********************************/

    return 0;
}

/**
 * @brief  读取红外温度值。
 * @para   存放红外温度数据的数组指针
 */
void mlx_getVal(float *celsius)
{
    u16 val;
    u8 buf[3];
    u8 pec_check[5];

    mlx_iic_read(MLX90614_DEV_ADDRESS, MLX90614_REG_TOBJ1, buf, 3);

    val = (u16)((buf[1] << 8) | buf[0]);

    if (celsius)
        *celsius = (val * 0.02f) - 273.15f;
}

#endif
