#include <stdio.h>
#include <math.h>
#include "stm32f10x.h"
#include "adxl345.h"
#include "ANALOG_I2C.h"
#include "delay.h"

//***************************
void  Init_ADXL345(void)
{
   Ana_I2C_E_W(ADXL345_Addr,0x31,0x0B);   //测量范围,正负16g，13位模式
  // Ana_I2C_E_W(ADXL345_Addr,0x2C,0x0e);   //速率设定为100hz 参考pdf13页
   Ana_I2C_E_W(ADXL345_Addr,0x2D,0x08);   //选择电源模式   参考pdf24页
   Ana_I2C_E_W(ADXL345_Addr,0x2E,0x80);   //使能 DATA_READY 中断
  // Ana_I2C_E_W(ADXL345_Addr,0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
  // Ana_I2C_E_W(ADXL345_Addr,0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
  // Ana_I2C_E_W(ADXL345_Addr,0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页
    
}

//****************************************
//读取3个轴的数据
//x,y,z:读取到的数据
void ADXL345_RD_XYZ(int16_t *x,int16_t *y,int16_t *z)
{
    uint8_t buf[6];

    buf[0]=Ana_I2C_E_R(ADXL345_Addr,0x32);//OUT_X_L_A
    buf[1]=Ana_I2C_E_R(ADXL345_Addr,0x33);//OUT_X_H_A

    buf[2]=Ana_I2C_E_R(ADXL345_Addr,0x34);//OUT_Y_L_A
    buf[3]=Ana_I2C_E_R(ADXL345_Addr,0x35);//OUT_Y_H_A

    buf[4]=Ana_I2C_E_R(ADXL345_Addr,0x36);//OUT_Z_L_A
    buf[5]=Ana_I2C_E_R(ADXL345_Addr,0x37);//OUT_Z_H_A
    
    
    *x=(int16_t)(((u16)buf[1]<<8)+buf[0]); //低字节先读，两个字节表示一个方向位的值    
    *y=(int16_t)(((u16)buf[3]<<8)+buf[2]);        
    *z=(int16_t)(((u16)buf[5]<<8)+buf[4]);       
}
//读取ADXL的平均值
//x,y,z:读取10次后取平均值
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

//得到角度
//x,y,z:x,y,z方向的重力加速度分量(不需要单位,直接数值即可)
//dir:要获得的角度.0,与Z轴的角度;1,与X轴的角度;2,与Y轴的角度.
//返回值:角度值.单位0.1°.
//res得到的是弧度值，需要将其转换为角度值也就是*180/3.14
int16_t ADXL345_Get_Angle(float x,float y,float z,uint8_t dir)
{
    float temp;
    float res=0;
    switch(dir)
    {
        case 0://与自然Z轴的角度
            temp=sqrt((x*x+y*y))/z;
            res=atan(temp);
            break;
        case 1://与自然X轴的角度
            temp=x/sqrt((y*y+z*z));
            res=atan(temp);
            break;
        case 2://与自然Y轴的角度
            temp=y/sqrt((x*x+z*z));
            res=atan(temp);
            break;
    }
    return res*180/3.14159;
}
//自动校准
//xval,yval,zval:x,y,z轴的校准值
void ADXL345_AUTO_Adjust(char *xval,char *yval,char *zval)
{
    short tx,ty,tz;
    u8 i;
    short offx=0,offy=0,offz=0;
    Ana_I2C_E_W(ADXL345_Addr,POWER_CTL,0x00);     //先进入休眠模式.
    delay_ms(100);
    Ana_I2C_E_W(ADXL345_Addr,DATA_FORMAT,0X2B);   //低电平中断输出,13位全分辨率,输出数据右对齐,16g量程 
    Ana_I2C_E_W(ADXL345_Addr,BW_RATE,0x0A);       //数据输出速度为100Hz
    Ana_I2C_E_W(ADXL345_Addr,POWER_CTL,0x28);     //链接使能,测量模式
    Ana_I2C_E_W(ADXL345_Addr,INT_ENABLE,0x00);    //不使用中断      
 
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
    ADXL345_RD_Avval(&A_X,&A_Y,&A_Z);//这个会取多次算平均值
    ADXL345_RD_XYZ(&A_X,&A_Y,&A_Z);

    printf( "Angle x = %d, ",ADXL345_Get_Angle(A_X,A_Y,A_Z,1));
    printf( "Angle y = %d, ",ADXL345_Get_Angle(A_X,A_Y,A_Z,2));
    printf( "Angle z = %d, \r\n",ADXL345_Get_Angle(A_X,A_Y,A_Z,3 ));
}
