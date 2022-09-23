/*
 * @Author: Jinsc
 * @Date: 2022-09-23 09:10:17
 * @LastEditors: Jinsc
 * @LastEditTime: 2022-09-23 09:53:45
 * @FilePath: \IIC_����������\HARDWARE\ANALOG_IIC\ANALOG_I2C.c
 * @Description: 
 * Copyright (c) 2022 by jinsc123654@gmail.com All Rights Reserved. 
 */
#include "stdio.h"
#include "delay.h"
#include "ANALOG_I2C.h"

//��ʼ��IIC
void Ana_I2C_Init(void)
{                         
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_I2C_PORT, ENABLE);/* ��GPIOʱ�� */

    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;/* ��©��� */
    GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);

    /* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
    Ana_I2C_Stop();
}
/**
  * @brief I2C��ʼ
  * @param ��
  * @retval ��
  */
void Ana_I2C_Start(void)
{
    SDA_OUT();     //sda�����
    I2C_SDA_1();            
    I2C_SCL_1();
    delay_us(4);
    I2C_SDA_0();//START:when CLK is high,DATA change form high to low 
    delay_us(4);
    I2C_SCL_0();//ǯסI2C���ߣ�׼�����ͻ�������� 
}
/**
  * @brief I2C����
  * @param ��
  * @retval ��
  */
void Ana_I2C_Stop(void)
{
    SDA_OUT();//sda�����
    I2C_SCL_0();
    I2C_SDA_0();//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    I2C_SCL_1(); 
    I2C_SDA_1();//����I2C���߽����ź�
    delay_us(4);
}
/**
  * @brief I2C����һ���ֽ�
  * @param DATA Ҫ���͵��ֽ�
  * @retval ��
  */
void Ana_I2C_SendByte(unsigned char DATA)
{
    u8 t;   
    SDA_OUT();         
    I2C_SCL_0();//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //I2C_SDA=(txd&0x80)>>7;
        if((DATA&0x80)>>7)
        I2C_SDA_1();
        else
        I2C_SDA_0();
        DATA<<=1;       
        delay_us(2);   //��TEA5767��������ʱ���Ǳ����
        I2C_SCL_1();
        delay_us(2); 
        I2C_SCL_0();    
        delay_us(2);
    }
}
/**
  * @brief I2C����һ���ֽ�
  * @param ��
  * @retval ���յ����ֽ�
  */
unsigned char Ana_I2C_ReceiveByte(void)
{
    unsigned char DATA = 0x00;
    unsigned char i;
    
    SDA_IN();//SDA����Ϊ����
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
  * @brief I2C����Ӧ��
  * @param AckBit Ӧ��λ, 0ΪӦ��, 1Ϊ��Ӧ��
  * @retval ��
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
  * @brief I2C����Ӧ��
  * @param ��
  * @retval Ӧ��λ, 0ΪӦ��, 1Ϊ��Ӧ��
  */
unsigned char Ana_I2C_ReceiveAck(void)
{
    u8 ucErrTime=0;
    SDA_IN();      //SDA����Ϊ����  
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
    I2C_SCL_0();//ʱ�����0        
    return 0;  
}
/**
  * @brief ɨ��һ��IIC����ID
  * @param ��
  * @retval ��ȡ���ĵ�һ������ID
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
  * @brief I2C���Ӧ�豸��Ӧλ��д��1���ֽ�
  * @param ADD �豸��ַ
  * @param WADD ���ݵ�ַ
  * @param DATA д�������
  * @retval ��
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
  * @brief I2C�Ӷ�Ӧ�豸��Ӧλ�ö�ȡ1���ֽ�
  * @param ADD �豸��ַ
  * @param WADD ���ݵ�ַ
  * @retval ��ȡ��������
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
 #ifdef NR_SHELL_USING_EXPORT_CMD /* ���ʹ��shell��� */
 /**
  * @brief   ע��һ��iic ɨ������
  * @example ��shell������ iic_scan����
  * @param   ��
  * @retval  ��
  */
static void iic_scan_cmd(char argc, char *argv)
{
    Ana_I2C_Dev_Add_Scan();
}
NR_SHELL_CMD_EXPORT(iic_scan, iic_scan_cmd);
#endif
