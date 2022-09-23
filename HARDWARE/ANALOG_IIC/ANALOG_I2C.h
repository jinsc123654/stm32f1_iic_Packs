/*
 * @Author: Jinsc
 * @Date: 2022-09-23 09:10:17
 * @LastEditors: Jinsc
 * @LastEditTime: 2022-09-23 09:54:01
 * @FilePath: \IIC_多器件例程\HARDWARE\ANALOG_IIC\ANALOG_I2C.h
 * @Description: 
 * Copyright (c) 2022 by jinsc123654@gmail.com All Rights Reserved. 
 */

#ifndef _BSP_ANA_IIC_H_
#define _BSP_ANA_IIC_H_
#include "stm32f10x.h"

#define IIC_TEST (0)
#if IIC_TEST 
//IO方向设置
/* 因为一个IO需要4bit设置所以有下方约定 */
/* 小于等于7的IO使用CRL 大于等于8的IO使用CRH */
/* 例如Pin1 就是 ~((uint32_t)0XF<<(4*1)) 与 3<<(4*1)*/
#define SDA_IN()  {GPIOB->CRL&=~((uint32_t)0XF<<(4*5));GPIOB->CRL|=(u32)8<<(4*5);}
#define SDA_OUT() {GPIOB->CRL&=~((uint32_t)0XF<<(4*5));GPIOB->CRL|=(u32)3<<(4*5);}



/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define GPIO_PORT_I2C	GPIOB			/* GPIO端口 */
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define I2C_SCL_PIN		GPIO_Pin_0			/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_PIN		GPIO_Pin_5			/* 连接到SDA数据线的GPIO */
#else
//IO方向设置
/* 因为一个IO需要4bit设置所以有下方约定 */
/* 小于等于7的IO使用CRL 大于等于8的IO使用CRH */
/* 例如Pin1 就是 ~((uint32_t)0XF<<(4*1)) 与 3<<(4*1)*/
#define SDA_IN()  {GPIOB->CRL&=~((uint32_t)0XF<<(4*7));GPIOB->CRL|=(u32)8<<(4*7);}
#define SDA_OUT() {GPIOB->CRL&=~((uint32_t)0XF<<(4*7));GPIOB->CRL|=(u32)3<<(4*7);}



/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define GPIO_PORT_I2C	GPIOB			/* GPIO端口 */
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define I2C_SCL_PIN		GPIO_Pin_6			/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_PIN		GPIO_Pin_7			/* 连接到SDA数据线的GPIO */
#endif

/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */

#define I2C_SCL_1()  GPIO_SetBits(GPIO_PORT_I2C, I2C_SCL_PIN)		/* SCL = 1 */
#define I2C_SCL_0()  GPIO_ResetBits(GPIO_PORT_I2C, I2C_SCL_PIN)     /* SCL = 0 */

#define I2C_SDA_1()  GPIO_SetBits(GPIO_PORT_I2C, I2C_SDA_PIN)		/* SDA = 1 */
#define I2C_SDA_0()  GPIO_ResetBits(GPIO_PORT_I2C, I2C_SDA_PIN)	    /* SDA = 0 */

#define I2C_SDA_READ()  GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SDA_PIN)	/* 读SDA口线状态 */
#define I2C_SCL_READ()  GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SCL_PIN)	/* 读SCL口线状态 */

//IIC所有操作函数
void            Ana_I2C_Init(void);                                                                 //初始化IIC的IO口
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
