#include "DS134_Color_drive.h"

#include <string.h>

#include "stc32g.h"
#include "sw_iic.h"
//#include "STC32G_I2C.h"
#include "STC32G_GPIO.h"
#include "STC32G_Delay.h"

#include "LED_drive.h"

//#include <math.h>

sbit INT = P1^1; //传感器的INT端口

#define FALSE 0
#define TRUE  1

tcs3400_data_t sensor;

static void tcs3400_init(void);
static void tcs3400_read_color(tcs3400_data_t *dev);
static void tcs3400_write_register(u8 reg, u8 val);
static void tcs3400_read_register(u8 reg, u8 *val);
static int8 tcs3400_read_data(u8 dev_addr, u8 reg_addr, u8 *dat, u16 len);

/**
  * @brief  初始化TCS3400传感器
  * @retval 成功返回0，失败返回1
  */
u8 DS134_Init(void)
{
/**********************************外设初始化**********************************/
    DIS_LED_init();
    DIS_LED_ALL_off();
    P1_MODE_IN_HIZ(GPIO_Pin_1); // P1.1设置为高阻输入
    soft_i2c_init();
/*********************************传感器初始化*********************************/
    tcs3400_init();

    return 0;
}

/**
  * @brief  读取R-G-B灯照射下，透射的R-G-B色度。
  * @para   存放R-G-B数据的数组指针
  */
void DS134_GetRGB(float *dat)
{
    DIS_LED_Just_One_Enable(1);  // 红色
    delay_ms(30);
    tcs3400_read_color(&sensor);
    dat[0] = sensor.red;

    DIS_LED_Just_One_Enable(2);  // 绿色
    delay_ms(30);
    tcs3400_read_color(&sensor);
    dat[1] = sensor.green;

    DIS_LED_Just_One_Enable(3);  // 蓝色
    delay_ms(30);
    tcs3400_read_color(&sensor);
    dat[2] = sensor.blue;
    
    DIS_LED_ALL_off();
}

static void tcs3400_init(void) 
{
    tcs3400_write_register(TCS3400_ENABLE, 0x00);
    // 设置积分时间
    tcs3400_write_register(TCS3400_ATIME, 0xFF);//1024 step
    // 设置增益
    tcs3400_write_register(TCS3400_CONTROL, 0x00);// 1x
    tcs3400_write_register(TCS3400_PERS, 0x0F);
    
//    // 设置最小门限
//    tcs3400_write_register(TCS3400_AILTL, 0x00);  // AILTL
//    tcs3400_write_register(TCS3400_AILTH, 0x00);  // AILTH
//    // 设置最大门限
//    tcs3400_write_register(TCS3400_AIHTL, 0xFF);  // AIHTL
//    tcs3400_write_register(TCS3400_AIHTH, 0xFF);  // AIHTH

    // 启用传感器和ADC
    tcs3400_write_register(TCS3400_ENABLE, TCS3400_ENABLE_PON);
    delay_ms(3);
    tcs3400_write_register(TCS3400_ENABLE, TCS3400_ENABLE_PON | TCS3400_ENABLE_AEN);

    delay_ms(150);
}

static void tcs3400_read_color(tcs3400_data_t *dev) 
{
    u8 status = 0;
    u8 buf[8] = {0};
    float rgb_sum = 0.0f;
    
    do{
    tcs3400_read_register(TCS3400_STATUS,&status);
    }
    while(!(status & 0x01));
    
    tcs3400_read_data(TCS3400_I2C_ADDR, TCS3400_CDATAL, buf, 8);
    
//    dev->clear = (buf[1] << 8) | buf[0];
    dev->red   = (buf[3] << 8) | buf[2];
    dev->green = (buf[5] << 8) | buf[4];
    dev->blue  = (buf[7] << 8) | buf[6];
    rgb_sum = dev->red + dev->green + dev->blue;
    if(rgb_sum != 0.0f)
    {
        dev->red = dev->red / rgb_sum * 100.0f;
        dev->green = dev->green / rgb_sum * 100.0f;
        dev->blue = dev->blue / rgb_sum * 100.0f;
    }
}

static void tcs3400_write_register(u8 reg, u8 val)
{
    soft_i2c_start();
    soft_i2c_write_byte(TCS3400_I2C_ADDR << 1);  // 写地址
    soft_i2c_write_byte(TCS3400_ENABLE | reg);
    soft_i2c_write_byte(val);
    soft_i2c_stop();
}

static void tcs3400_read_register(u8 reg, u8 *val)
{
    soft_i2c_start();
    soft_i2c_write_byte(TCS3400_I2C_ADDR << 1);  // 写地址
    soft_i2c_write_byte(reg);
    soft_i2c_start();                   // 重启
    soft_i2c_write_byte((TCS3400_I2C_ADDR << 1) | 1);  // 读地址
    *val = soft_i2c_read_byte(NACK);    // 读 + NACK
    soft_i2c_stop();
}

//先写目标寄存器再读数据
static int8 tcs3400_read_data(u8 dev_addr, u8 reg_addr, u8 *dat, u16 len)
{
    u16 i;
    soft_i2c_start();
    if (!soft_i2c_write_byte(dev_addr << 1)) // 写地址
    {
        soft_i2c_stop();
        return -1;
    }
    if (!soft_i2c_write_byte(reg_addr)) // 写寄存器
    {
        soft_i2c_stop();
        return -2;
    }
    soft_i2c_start();
    
    if (!soft_i2c_write_byte((dev_addr << 1) | 0x01)) // 读地址
    {
        soft_i2c_stop();
        return -3;
    }
    for (i = 0; i < len; i++) {
        dat[i] = soft_i2c_read_byte(i < (len - 1)); // 最后一个发NACK
    }
    soft_i2c_stop();
    return 0;
}