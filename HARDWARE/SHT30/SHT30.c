#include "ANALOG_I2C.h"
#include "SHT30.h"
#include "delay.h"
#include "stdio.h"

//*********************************************************************************
//SHT30����
//�Ա�����:https://shop275468129.taobao.com/
//�޸�����:2018/6/25
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ���ݴ�о���� 2018-2028
//All rights reserved
//*********************************************************************************


#define POLYNOMIAL_CXDZ 0x31 // X^8 + X^5 + X^4 + 1
//SHT3X CRCУ��
unsigned char SHT3X_CRC(u8 *data, u8 len)
{
    unsigned char bit;        // bit mask
    unsigned char crc = 0xFF; // calculated checksum
    unsigned char byteCtr;    // byte counter

    // calculates 8-Bit checksum with given polynomial @GZCXDZ
    for(byteCtr = 0; byteCtr < len; byteCtr++) {
            crc ^= (data[byteCtr]);
            for(bit = 8; bit > 0; --bit) {
                    if(crc & 0x80) {
                            crc = (crc << 1) ^ POLYNOMIAL_CXDZ;
                    }  else {
                            crc = (crc << 1);
                    }
            }
    }
  return crc;
}

//SHT30�����
//addr:��ʾ��Ʒ����ţ���ΪSHT30ʹ��IIC���ߵĻ�һ�����Ͽ��Թ�����
void SHT30_CMD(u16 cmd)
{
    Ana_I2C_Start();
    Ana_I2C_SendByte(SHT30_ADDR+0);  //�����豸��ַ,д�Ĵ���
    Ana_I2C_ReceiveAck();
    Ana_I2C_SendByte((cmd>>8)&0xff); //MSB
    Ana_I2C_ReceiveAck();
    Ana_I2C_SendByte(cmd&0xff); //LSB
    Ana_I2C_ReceiveAck();
    Ana_I2C_Stop();
    delay_ms(50);//��������Ҫ�ȴ�20ms���ϲ��ܶ�д
}



//SHT30��ȡ��ʪ��
//temp:�¶ȣ�-400~1250��ʵ���¶�=temp/10,�ֱ���0.1��,���ȡ�0.3��
//humi:ʪ�ȣ�0~1000��ʵ��ʪ��=humi/10,�ֱ���0.1%rh,���ȡ�3
//����0�ɹ���1ʧ��
u8 SHT30_Read_Humiture(int *temp,u16 *humi)
{
    u8 buff[6];
    
    SHT30_CMD(SHT30_READ_HUMITURE);//����ʪ������
    
    Ana_I2C_Start();
    Ana_I2C_SendByte(SHT30_ADDR+1); //�����豸��ַ,���Ĵ���
    Ana_I2C_ReceiveAck();
    buff[0]=Ana_I2C_ReceiveByte();Ana_I2C_SendAck(0);//����������Ӧ��
    buff[1]=Ana_I2C_ReceiveByte();Ana_I2C_SendAck(0);//����������Ӧ��
    buff[2]=Ana_I2C_ReceiveByte();Ana_I2C_SendAck(0);//����������Ӧ��
    buff[3]=Ana_I2C_ReceiveByte();Ana_I2C_SendAck(0);//����������Ӧ��
    buff[4]=Ana_I2C_ReceiveByte();Ana_I2C_SendAck(0);//����������Ӧ��
    buff[5]=Ana_I2C_ReceiveByte();Ana_I2C_SendAck(1);//��������ֹͣӦ��
    Ana_I2C_Stop();

    
    //printf("buff=%d,%d,%d,%d,%d,%d\r\n",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]);
    //CRCУ��
    if(SHT3X_CRC(&buff[0],2)==buff[2] && SHT3X_CRC(&buff[3],2)==buff[5])
    {

        *temp=(-45+(175.0*((buff[0]<<8)+buff[1])/65535.0))*10;
        *humi=10*100*((buff[3]<<8)+buff[4])/65535.0;
        if(*temp>1250) *temp=1250;
        else if(*temp<-400) *temp=-400;
        return 0;
    }
    else return 1;    
    
}

//SHT30��ʼ��
void SHT30_Init()
{
    Ana_I2C_Init();
}


