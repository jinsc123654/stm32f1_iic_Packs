/*
 * @Author: Jinsc
 * @Date: 2022-09-23 09:10:17
 * @LastEditors: Jinsc
 * @LastEditTime: 2022-09-23 09:53:45
 * @FilePath: \IIC_多器件例程\HARDWARE\ANALOG_IIC\ANALOG_I2C.c
 * @Description: 
 * Copyright (c) 2022 by jinsc123654@gmail.com All Rights Reserved. 
 */
#include "stdio.h"
#include "delay.h"
#include "ANALOG_I2C.h"

//初始化IIC
void Ana_I2C_Init(void)
{                         
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_I2C_PORT, ENABLE);/* 打开GPIO时钟 */

    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;/* 开漏输出 */
    GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);

    /* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
    Ana_I2C_Stop();
}
/**
  * @brief I2C开始
  * @param 无
  * @retval 无
  */
void Ana_I2C_Start(void)
{
    SDA_OUT();     //sda线输出
    I2C_SDA_1();            
    I2C_SCL_1();
    delay_us(4);
    I2C_SDA_0();//START:when CLK is high,DATA change form high to low 
    delay_us(4);
    I2C_SCL_0();//钳住I2C总线，准备发送或接收数据 
}
/**
  * @brief I2C结束
  * @param 无
  * @retval 无
  */
void Ana_I2C_Stop(void)
{
    SDA_OUT();//sda线输出
    I2C_SCL_0();
    I2C_SDA_0();//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    I2C_SCL_1(); 
    I2C_SDA_1();//发送I2C总线结束信号
    delay_us(4);
}
/**
  * @brief I2C发送一个字节
  * @param DATA 要发送的字节
  * @retval 无
  */
void Ana_I2C_SendByte(unsigned char DATA)
{
    u8 t;   
    SDA_OUT();         
    I2C_SCL_0();//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //I2C_SDA=(txd&0x80)>>7;
        if((DATA&0x80)>>7)
        I2C_SDA_1();
        else
        I2C_SDA_0();
        DATA<<=1;       
        delay_us(2);   //对TEA5767这三个延时都是必须的
        I2C_SCL_1();
        delay_us(2); 
        I2C_SCL_0();    
        delay_us(2);
    }
}
/**
  * @brief I2C接收一个字节
  * @param 无
  * @retval 接收到的字节
  */
unsigned char Ana_I2C_ReceiveByte(void)
{
    unsigned char DATA = 0x00;
    unsigned char i;
    
    SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
    {
        I2C_SCL_0(); 
        delay_us(2);
        I2C_SCL_1();
        DATA<<=1;
        if(I2C_SDA_READ())DATA++;   
        delay_us(1); 
    }                     
    
    return DATA;
}
/**
  * @brief I2C发送应答
  * @param AckBit 应答位, 0为应答, 1为非应答
  * @retval 无
  */
void Ana_I2C_SendAck(unsigned char AckBit)
{
    I2C_SCL_0();
    SDA_OUT();
    if(AckBit == 1)
    {
        I2C_SDA_1();
    }
    else
    {
        I2C_SDA_0();
    }
    delay_us(2);
    I2C_SCL_1();
    delay_us(2);
    I2C_SCL_0();
}
/**
  * @brief I2C接收应答
  * @param 无
  * @retval 应答位, 0为应答, 1为非应答
  */
unsigned char Ana_I2C_ReceiveAck(void)
{
    u8 ucErrTime=0;
    SDA_IN();      //SDA设置为输入  
    I2C_SDA_1();delay_us(1);       
    I2C_SCL_1();delay_us(1);     
    while(I2C_SDA_READ())
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            Ana_I2C_Stop();
            return 1;
        }
    }
    I2C_SCL_0();//时钟输出0        
    return 0;  
}
/**
  * @brief 扫描一次IIC器件ID
  * @param 无
  * @retval 读取到的第一个器件ID
  */
uint8_t Ana_I2C_Dev_Add_Scan(void)
{
    uint8_t addr = 0;
    uint8_t ack;
    uint8_t read = 0;
    uint8_t res_id = 255;
    printf("\r\nStart scanning IIC devices\r\n");
    for(addr = 0 ; addr < 255 ;addr++)
    {
        Ana_I2C_Start();
        Ana_I2C_SendByte(addr);
        ack = Ana_I2C_ReceiveAck();
        if(ack == 0 && (read %2 ==0) )
        {
            printf("\r\nwrite addr = 0x%x OK ",addr);
            read++;
            if( res_id == 255 && addr!=0 )
            {
                res_id = addr;
            }
        }
        else if(ack ==0 && read %2 == 1)
        {
            printf(" -> read addr = 0x%x\r\n",addr);
            read++;
        }
        Ana_I2C_Stop();
    }
    
    return res_id;
}

/**
  * @brief I2C向对应设备对应位置写入1个字节
  * @param ADD 设备地址
  * @param WADD 数据地址
  * @param DATA 写入的数据
  * @retval 无
  */
void Ana_I2C_E_W(unsigned char ADD, unsigned char WADD, unsigned char DATA)
{
    Ana_I2C_Start();
    Ana_I2C_SendByte(ADD);
    Ana_I2C_ReceiveAck();
    Ana_I2C_SendByte(WADD);
    Ana_I2C_ReceiveAck();
    Ana_I2C_SendByte(DATA);
    Ana_I2C_ReceiveAck();
    Ana_I2C_Stop();
    delay_us(5);
}

/**
  * @brief I2C从对应设备对应位置读取1个字节
  * @param ADD 设备地址
  * @param WADD 数据地址
  * @retval 读取到的数据
  */
unsigned char Ana_I2C_E_R(unsigned char ADD, unsigned char WADD)
{
    unsigned char DATA;
    Ana_I2C_Start();
    Ana_I2C_SendByte(ADD);
    Ana_I2C_ReceiveAck();
    Ana_I2C_SendByte(WADD);
    Ana_I2C_ReceiveAck();

    Ana_I2C_Start();
    Ana_I2C_SendByte(ADD | 0x01);
    Ana_I2C_ReceiveAck();
    DATA = Ana_I2C_ReceiveByte();
    Ana_I2C_SendAck(0x01);
    Ana_I2C_Stop();
    return DATA;
}
#include "./nr_micro_shell/inc/my_nr_micro_shell.h"
 #ifdef NR_SHELL_USING_EXPORT_CMD /* 如果使用shell组件 */
 /**
  * @brief   注册一个iic 扫描命令
  * @example 在shell中输入 iic_scan即可
  * @param   无
  * @retval  无
  */
static void iic_scan_cmd(char argc, char *argv)
{
    Ana_I2C_Dev_Add_Scan();
}
NR_SHELL_CMD_EXPORT(iic_scan, iic_scan_cmd);
#endif
