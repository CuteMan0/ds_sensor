#include "type_def.h"

#define SDA_OUT()     P2M1 &= ~(1<<4); P2M0 |= (1<<4)  // 设置P2.4为推挽输出
#define SDA_IN()      P2M1 |= (1<<4);  P2M0 &= ~(1<<4) // 设置P2.4为输入
#define SDA_HIGH()    P2 |=  (1<<4)
#define SDA_LOW()     P2 &= ~(1<<4)
#define SDA_READ      (P2 & (1<<4))

#define SCL_OUT()     P2M1 &= ~(1<<5); P2M0 |= (1<<5)  // 设置P2.5为推挽输出
#define SCL_HIGH()    P2 |=  (1<<5)
#define SCL_LOW()     P2 &= ~(1<<5)

#define ACK  1
#define NACK 0

void soft_i2c_delay(void);

// 初始化GPIO
void soft_i2c_init(void);

void soft_i2c_start(void);
void soft_i2c_stop(void);

bit soft_i2c_write_byte(u8 byte);
u8 soft_i2c_read_byte(bit ack);