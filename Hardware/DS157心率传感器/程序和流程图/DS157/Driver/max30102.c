#include "max30102.h"

#include "sw_iic.h"
#include "STC32G_Delay.h"

// 传感器初始化标志
static uint8_t g_max30102_inited = 0;

// 内部I2C读写函数 
uint8_t max30102_readBytes(uint8_t reg_addr, uint8_t *buf, uint8_t len)
{
    return soft_iic_read(MAX30102_I2C_ADDRESS,reg_addr,buf,len);
}

uint8_t max30102_writeByte(uint8_t reg_addr, uint8_t dat)
{
    return soft_iic_write(MAX30102_I2C_ADDRESS,reg_addr,&dat,1);
}

/**
 * @brief 初始化MAX30102
 * @return 0-成功, 非0-失败
 */
uint8_t max30102_init(void)
{
    uint8_t res, part_id, prev,tmp;
    
    soft_i2c_init();
    // 检查器件ID
    res = max30102_readBytes(MAX30102_REG_PART_ID, &part_id, 1);
    if (res != 0 || part_id != 0x15) {
        return 1; // ID错误
    }
    
    // 软复位 (置位RST位)
    res = max30102_readBytes(MAX30102_REG_MODE_CONFIG, &prev, 1);
    if (res != 0) return 2;
    prev |= (1 << 6); // 设置RST位
    res = max30102_writeByte(MAX30102_REG_MODE_CONFIG, prev);
    if (res != 0) return 2;
    delay_ms(10); // 等待复位完成
    
    // 配置传感器工作模式、采样率、设置LED电流等
    // 例如: 采样率、使能中断等
    max30102_writeByte(MAX30102_REG_INTERRUPT_ENABLE_1,0xE0);
    max30102_writeByte(MAX30102_REG_INTERRUPT_ENABLE_2,0x02);

    // 复位FIFO (将读/写指针和溢出计数器清零)
    prev = 0x00;
    res = max30102_writeByte(MAX30102_REG_FIFO_READ_POINTER, prev);
    if (res != 0) return 3;
    res = max30102_writeByte(MAX30102_REG_FIFO_WRITE_POINTER, prev);
    if (res != 0) return 3;
    res = max30102_writeByte(MAX30102_REG_OVERFLOW_COUNTER, prev);
    if (res != 0) return 3;
    
    max30102_writeByte(MAX30102_REG_FIFO_CONFIG, 0x0F);

    max30102_writeByte(MAX30102_REG_MODE_CONFIG, MAX30102_MODE_SPO2);
    
    //SpO2 configuration:ACD resolution:15.63pA,sample rate control: 50Hz, LED pulse width:215 us 
    max30102_writeByte(MAX30102_REG_SPO2_CONFIG,0x23);

    max30102_writeByte(MAX30102_REG_LED_PULSE_1,0x3F);
    max30102_writeByte(MAX30102_REG_LED_PULSE_2,0x3F);
    max30102_writeByte(MAX30102_REG_PILOT_LED_PULSE,0x7f);
    
    max30102_writeByte(MAX30102_REG_DIE_TEMP_CONFIG,0x01);  //temperture
    
    // 清除任何挂起的中断
    max30102_readBytes(MAX30102_REG_INTERRUPT_STATUS_1, &tmp, 1);
    max30102_readBytes(MAX30102_REG_INTERRUPT_STATUS_2, &tmp, 1);

    g_max30102_inited = 1;
    return 0;
}


/**
 * @brief 从FIFO读取数据
 */
void max30102_readFIFO(uint32_t *raw_red, uint32_t *raw_ir)
{
    u8 receive_data[6]={0};
    u8 temp_data=0;
    u32 dat[2];
    u32 timeout = 3;

    do{
        max30102_readBytes(MAX30102_REG_INTERRUPT_STATUS_1,&temp_data,1);
        // 多次失败就返回
        if (--timeout == 0)break;
    }while((temp_data&0x40)!=0x40);
    max30102_readBytes(MAX30102_REG_FIFO_DATA_REGISTER,receive_data,6);
    dat[0] = ((receive_data[0] << 16) | 
              (receive_data[1] << 8)  | 
               receive_data[2]) & 0x03FFFF;

    dat[1] = ((receive_data[3] << 16) | 
              (receive_data[4] << 8)  | 
               receive_data[5]) & 0x03FFFF;
    *raw_red = dat[0];
    *raw_ir = dat[1];
}
