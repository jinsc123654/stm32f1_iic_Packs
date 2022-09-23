#ifndef __adxl345_H
#define __adxl345_H
#include "stdint.h"
#include "stdbool.h"

#define	ADXL345_Addr    0xa6	//加速度传感器器件地址

#define BW_RATE12D5				0x07
#define BW_RATE25				0x08
#define BW_RATE50				0x09
#define BW_RATE100				0x0A
#define BW_RATE200				0x0B
#define BW_RATE400				0x0C
#define BW_RATE800				0x0D
#define BW_RATE1600				0x0E
#define BW_RATE3200				0x0F


#define DEVICE_ID		0x00 	//器件ID, 读数0xE5
#define THRESH_TAP		0X1D   	//敲击阀值
#define OFSX			0X1E
#define OFSY			0X1F
#define OFSZ			0X20
#define DUR				0X21
#define Latent			0X22
#define Window  		0X23 
#define THRESH_ACT		0X24
#define THRESH_INACT	0X25 
#define TIME_INACT		0X26
#define ACT_INACT_CTL	0X27
#define THRESH_FF		0X28
#define TIME_FF			0X29
#define TAP_AXES		0X2A
#define ACT_TAP_STATUS  0X2B 
#define BW_RATE			0X2C 
#define POWER_CTL		0X2D 

#define INT_ENABLE		0X2E
#define INT_MAP			0X2F
#define INT_SOURCE  	0X30
#define DATA_FORMAT     0X31
#define DATA_X0			0X32
#define DATA_X1			0X33
#define DATA_Y0			0X34
#define DATA_Y1			0X35
#define DATA_Z0			0X36
#define DATA_Z1			0X37
#define FIFO_CTL		0X38
#define FIFO_STATUS		0X39


void Init_ADXL345(void);
void ADXL345_RD_XYZ(int16_t *x,int16_t *y,int16_t *z);
void ADXL345_RD_Avval(int16_t *x,int16_t *y,int16_t *z);
int16_t ADXL345_Get_Angle(float x,float y,float z,uint8_t dir);
void ADXL345_AUTO_Adjust(char *xval,char *yval,char *zval);
void  Send_ADXL345(void);

#endif
