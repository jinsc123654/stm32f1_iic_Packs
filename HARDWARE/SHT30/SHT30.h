#ifndef __SHT30_H
#define __SHT30_H 
#include "stm32f10x.h"  

//*********************************************************************************
//SHT30����
//�Ա�����:https://shop275468129.taobao.com/
//�޸�����:2018/6/25
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ���ݴ�о���� 2018-2028
//All rights reserved
//*********************************************************************************



#define SHT30_ADDR (u8)(0x44<<1) //sht30 i2c��ַ��ADDR�ܽŽӵ͵�ƽʱΪ0x44,�Ӹߵ�ƽΪ0x45

//SHT30����
#define SHT30_READ_HUMITURE (u16)0x2c06  //����ʪ��

//CRC����ʽ
#define POLYNOMIAL 0x31 // X^8 + X^5 + X^4 + 1

u8 SHT30_Read_Humiture(int *temp,u16 *humi);
void SHT30_Init(void);

#endif


