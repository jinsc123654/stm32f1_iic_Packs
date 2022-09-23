#include <stdio.h>
#include <math.h>
#include "stm32f10x.h"
#include "adxl345.h"
#include "ANALOG_I2C.h"
#include "delay.h"

//***************************
void  Init_ADXL345(void)
{
   Ana_I2C_E_W(ADXL345_Addr,0x31,0x0B);   //������Χ,����16g��13λģʽ
  // Ana_I2C_E_W(ADXL345_Addr,0x2C,0x0e);   //�����趨Ϊ100hz �ο�pdf13ҳ
   Ana_I2C_E_W(ADXL345_Addr,0x2D,0x08);   //ѡ���Դģʽ   �ο�pdf24ҳ
   Ana_I2C_E_W(ADXL345_Addr,0x2E,0x80);   //ʹ�� DATA_READY �ж�
  // Ana_I2C_E_W(ADXL345_Addr,0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
  // Ana_I2C_E_W(ADXL345_Addr,0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
  // Ana_I2C_E_W(ADXL345_Addr,0x20,0x05);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
    
}

//****************************************
//��ȡ3���������
//x,y,z:��ȡ��������
void ADXL345_RD_XYZ(int16_t *x,int16_t *y,int16_t *z)
{
    uint8_t buf[6];

    buf[0]=Ana_I2C_E_R(ADXL345_Addr,0x32);//OUT_X_L_A
    buf[1]=Ana_I2C_E_R(ADXL345_Addr,0x33);//OUT_X_H_A

    buf[2]=Ana_I2C_E_R(ADXL345_Addr,0x34);//OUT_Y_L_A
    buf[3]=Ana_I2C_E_R(ADXL345_Addr,0x35);//OUT_Y_H_A

    buf[4]=Ana_I2C_E_R(ADXL345_Addr,0x36);//OUT_Z_L_A
    buf[5]=Ana_I2C_E_R(ADXL345_Addr,0x37);//OUT_Z_H_A
    
    
    *x=(int16_t)(((u16)buf[1]<<8)+buf[0]); //���ֽ��ȶ��������ֽڱ�ʾһ������λ��ֵ    
    *y=(int16_t)(((u16)buf[3]<<8)+buf[2]);        
    *z=(int16_t)(((u16)buf[5]<<8)+buf[4]);       
}
//��ȡADXL��ƽ��ֵ
//x,y,z:��ȡ10�κ�ȡƽ��ֵ
void ADXL345_RD_Avval(int16_t *x,int16_t *y,int16_t *z)
{
    int16_t tx=0,ty=0,tz=0;      
    uint8_t i;  
    for(i=0;i<10;i++)
    {
        ADXL345_RD_XYZ(x,y,z);
        delay_ms(10);
        tx+=(int16_t)*x;
        ty+=(int16_t)*y;
        tz+=(int16_t)*z;     
    }
    *x=tx/10;
    *y=ty/10;
    *z=tz/10;
}

//�õ��Ƕ�
//x,y,z:x,y,z������������ٶȷ���(����Ҫ��λ,ֱ����ֵ����)
//dir:Ҫ��õĽǶ�.0,��Z��ĽǶ�;1,��X��ĽǶ�;2,��Y��ĽǶ�.
//����ֵ:�Ƕ�ֵ.��λ0.1��.
//res�õ����ǻ���ֵ����Ҫ����ת��Ϊ�Ƕ�ֵҲ����*180/3.14
int16_t ADXL345_Get_Angle(float x,float y,float z,uint8_t dir)
{
    float temp;
    float res=0;
    switch(dir)
    {
        case 0://����ȻZ��ĽǶ�
            temp=sqrt((x*x+y*y))/z;
            res=atan(temp);
            break;
        case 1://����ȻX��ĽǶ�
            temp=x/sqrt((y*y+z*z));
            res=atan(temp);
            break;
        case 2://����ȻY��ĽǶ�
            temp=y/sqrt((x*x+z*z));
            res=atan(temp);
            break;
    }
    return res*180/3.14159;
}
//�Զ�У׼
//xval,yval,zval:x,y,z���У׼ֵ
void ADXL345_AUTO_Adjust(char *xval,char *yval,char *zval)
{
    short tx,ty,tz;
    u8 i;
    short offx=0,offy=0,offz=0;
    Ana_I2C_E_W(ADXL345_Addr,POWER_CTL,0x00);     //�Ƚ�������ģʽ.
    delay_ms(100);
    Ana_I2C_E_W(ADXL345_Addr,DATA_FORMAT,0X2B);   //�͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g���� 
    Ana_I2C_E_W(ADXL345_Addr,BW_RATE,0x0A);       //��������ٶ�Ϊ100Hz
    Ana_I2C_E_W(ADXL345_Addr,POWER_CTL,0x28);     //����ʹ��,����ģʽ
    Ana_I2C_E_W(ADXL345_Addr,INT_ENABLE,0x00);    //��ʹ���ж�      
 
    Ana_I2C_E_W(ADXL345_Addr,OFSX,0x00);
    Ana_I2C_E_W(ADXL345_Addr,OFSY,0x00);
    Ana_I2C_E_W(ADXL345_Addr,OFSZ,0x00);
    delay_ms(12);
    for(i=0;i<10;i++)
    {
        ADXL345_RD_Avval(&tx,&ty,&tz);
        offx+=tx;
        offy+=ty;
        offz+=tz;
    }           
    offx/=10;
    offy/=10;
    offz/=10;
    *xval=-offx/4;
    *yval=-offy/4;
    *zval=-(offz-256)/4;      
    Ana_I2C_E_W(ADXL345_Addr,OFSX,*xval);
    Ana_I2C_E_W(ADXL345_Addr,OFSY,*yval);
    Ana_I2C_E_W(ADXL345_Addr,OFSZ,*zval); 
}
void Send_ADXL345(void)
{
    int16_t  A_X,A_Y,A_Z;
    ADXL345_RD_Avval(&A_X,&A_Y,&A_Z);//�����ȡ�����ƽ��ֵ
    ADXL345_RD_XYZ(&A_X,&A_Y,&A_Z);

    printf( "Angle x = %d, ",ADXL345_Get_Angle(A_X,A_Y,A_Z,1));
    printf( "Angle y = %d, ",ADXL345_Get_Angle(A_X,A_Y,A_Z,2));
    printf( "Angle z = %d, \r\n",ADXL345_Get_Angle(A_X,A_Y,A_Z,3 ));
}
