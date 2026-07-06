//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: i2c.c
// 文件描述: 
// 文件版本: V1.0
// 修改记录:
//   1. (2026-07-06) 创建文件
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#include "config.h"


//<<AICUBE_USER_INCLUDE_BEGIN>>
// 在此添加用户头文件包含  
//<<AICUBE_USER_INCLUDE_END>>


//<<AICUBE_USER_GLOBAL_DEFINE_BEGIN>>
// 在此添加用户全局变量定义、用户宏定义以及函数声明  
//<<AICUBE_USER_GLOBAL_DEFINE_END>>



////////////////////////////////////////
// I2C初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void I2C_Init(void)
{
    I2C_SwitchP2324();                  //选择I2C数据口: SCL(P2.4), SDA(P2.3)

    I2C_MasterMode();                   //设置I2C为主机模式
    I2C_SetClockDivider(23);            //设置I2C为主机模式时钟

    I2C_Enable();                       //使能I2C功能

    //<<AICUBE_USER_I2C_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_I2C_INITIAL_END>>
}

////////////////////////////////////////
// I2C主机模式等待命令完成
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void I2C_MasterWait(void)
{
    while (!I2C_CheckMasterFlag());     //等待完成标志
    I2C_ClearMasterFlag();              //清除完成标志
    I2C_Idle();                         //恢复IDLE状态
}

////////////////////////////////////////
// I2C主机模式发送起始信号
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void I2C_MasterStart(void)
{
    I2C_Start();                        //触发主机模式起始命令
    I2C_MasterWait();                   //等待命令完成
}

////////////////////////////////////////
// I2C主机模式发送停止信号
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void I2C_MasterStop(void)
{
    I2C_Stop();                         //触发主机模式停止命令
    I2C_MasterWait();                   //等待命令完成
}

////////////////////////////////////////
// I2C主机模式发送起始信号+1字节数据
// 入口参数: dat (待发送的字节数据)
// 函数返回: 0   (接收的应答信号为ACK)
//           1   (接收的应答信号为NAK)
////////////////////////////////////////
BOOL I2C_MasterStartSendByte(uint8_t dat)
{
    I2C_WriteData(dat);                 //将数据写入I2C数据寄存器
    I2C_StartSendDataRecvACK();         //触发主机模式起始信号+写数据+读取ACK组合命令
    I2C_MasterWait();                   //等待命令完成

    return I2C_MasterReadACK();         //读取并返回应答信号
}

////////////////////////////////////////
// I2C主机模式发送1字节数据
// 入口参数: dat (待发送的字节数据)
// 函数返回: 0   (接收的应答信号为ACK)
//           1   (接收的应答信号为NAK)
////////////////////////////////////////
BOOL I2C_MasterSendByte(uint8_t dat)
{
    I2C_WriteData(dat);                 //将数据写入I2C数据寄存器
    I2C_SendDataRecvACK();              //触发主机模式写数据+读取ACK组合命令
    I2C_MasterWait();                   //等待命令完成

    return I2C_MasterReadACK();         //读取并返回应答信号
}

////////////////////////////////////////
// I2C主机模式接收1字节数据
// 入口参数: ack (待发送的应答信号)
// 函数返回:     (接收的字节数据)
////////////////////////////////////////
uint8_t I2C_MasterReadByte(BOOL ack)
{
    uint8_t dat;

    if (!ack)
        I2C_RecvDataSendACK();          //触发主机模式读数据+返回ACK命令
    else
        I2C_RecvDataSendNAK();          //触发主机模式读数据+返回NAK命令
    I2C_MasterWait();                   //等待命令完成
    dat = I2C_ReadData();               //读取接收的数据

    return dat;                         //返回接收的数据
}



//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码  
//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


