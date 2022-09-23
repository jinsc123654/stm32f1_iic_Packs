/*
 * @Author: Jinsc
 * @Date: 2022-09-23 09:10:17
 * @LastEditors: Jinsc
 * @LastEditTime: 2022-09-23 09:54:01
 * @FilePath: \IIC_����������\HARDWARE\ANALOG_IIC\ANALOG_I2C.h
 * @Description: 
 * Copyright (c) 2022 by jinsc123654@gmail.com All Rights Reserved. 
 */

#ifndef _BSP_ANA_IIC_H_
#define _BSP_ANA_IIC_H_
#include "stm32f10x.h"

#define IIC_TEST (0)
#if IIC_TEST 
//IO��������
/* ��Ϊһ��IO��Ҫ4bit�����������·�Լ�� */
/* С�ڵ���7��IOʹ��CRL ���ڵ���8��IOʹ��CRH */
/* ����Pin1 ���� ~((uint32_t)0XF<<(4*1)) �� 3<<(4*1)*/
#define SDA_IN()  {GPIOB->CRL&=~((uint32_t)0XF<<(4*5));GPIOB->CRL|=(u32)8<<(4*5);}
#define SDA_OUT() {GPIOB->CRL&=~((uint32_t)0XF<<(4*5));GPIOB->CRL|=(u32)3<<(4*5);}



/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define GPIO_PORT_I2C	GPIOB			/* GPIO�˿� */
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define I2C_SCL_PIN		GPIO_Pin_0			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN		GPIO_Pin_5			/* ���ӵ�SDA�����ߵ�GPIO */
#else
//IO��������
/* ��Ϊһ��IO��Ҫ4bit�����������·�Լ�� */
/* С�ڵ���7��IOʹ��CRL ���ڵ���8��IOʹ��CRH */
/* ����Pin1 ���� ~((uint32_t)0XF<<(4*1)) �� 3<<(4*1)*/
#define SDA_IN()  {GPIOB->CRL&=~((uint32_t)0XF<<(4*7));GPIOB->CRL|=(u32)8<<(4*7);}
#define SDA_OUT() {GPIOB->CRL&=~((uint32_t)0XF<<(4*7));GPIOB->CRL|=(u32)3<<(4*7);}



/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define GPIO_PORT_I2C	GPIOB			/* GPIO�˿� */
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define I2C_SCL_PIN		GPIO_Pin_6			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN		GPIO_Pin_7			/* ���ӵ�SDA�����ߵ�GPIO */
#endif

/* �����дSCL��SDA�ĺ꣬�����Ӵ���Ŀ���ֲ�ԺͿ��Ķ��� */

#define I2C_SCL_1()  GPIO_SetBits(GPIO_PORT_I2C, I2C_SCL_PIN)		/* SCL = 1 */
#define I2C_SCL_0()  GPIO_ResetBits(GPIO_PORT_I2C, I2C_SCL_PIN)     /* SCL = 0 */

#define I2C_SDA_1()  GPIO_SetBits(GPIO_PORT_I2C, I2C_SDA_PIN)		/* SDA = 1 */
#define I2C_SDA_0()  GPIO_ResetBits(GPIO_PORT_I2C, I2C_SDA_PIN)	    /* SDA = 0 */

#define I2C_SDA_READ()  GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SDA_PIN)	/* ��SDA����״̬ */
#define I2C_SCL_READ()  GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SCL_PIN)	/* ��SCL����״̬ */

//IIC���в�������
void            Ana_I2C_Init(void);                                                                 //��ʼ��IIC��IO��
void            Ana_I2C_Start(void);
void            Ana_I2C_Stop(void);
void            Ana_I2C_SendByte(unsigned char DATA);
unsigned char   Ana_I2C_ReceiveByte(void);
void            Ana_I2C_SendAck(unsigned char AckBit);
unsigned char   Ana_I2C_ReceiveAck(void);
uint8_t         Ana_I2C_Dev_Add_Scan(void);
unsigned char   Ana_I2C_E_R(unsigned char ADD, unsigned char WADD);
void            Ana_I2C_E_W(unsigned char ADD, unsigned char WADD, unsigned char DATA);
#endif
