/*
 * @Author: Jinsc
 * @Date: 2022-09-23 09:10:17
 * @LastEditors: Jinsc
 * @LastEditTime: 2022-09-23 09:52:58
 * @FilePath: \IIC_����������\USER\main.c
 * @Description: 
 * Copyright (c) 2022 by jinsc123654@gmail.com All Rights Reserved. 
 */

#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "./nr_micro_shell/inc/my_nr_micro_shell.h"
#include "timer.h"
#include "ANALOG_I2C.h"
#include "PAJ7620.h"
#include "SHT30.h"
#include "LIS2DH12.h"
#include "BMP280.h"
#include "adxl345.h"
#include "fdc2214.h"

float fliter(float pre, float cur)
{
    return 0.2 * pre + 0.8 * cur;
}

 int main(void)
{        
    uint16_t read_id = 0;
    delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);
    my_shell_init();        /* ��ʼ��shell */
    TIM3_Int_Init(100,7199);/*ʹ�ö�ʱ����shell���лص� 100��10ms*/
    Ana_I2C_Init();
    read_id = Ana_I2C_Dev_Add_Scan();
//  read_id = BMP280_ADDR;//

    float fdc_res_off[4];

    switch( read_id )
    {
        case PAJ7620_ID:
        {
            printf( "����ʹ�õ�Ӳ����PAJ7620\r\n");
            PAJ7620_Init();
        }
        break;
        case SHT30_ADDR+1://��1����ΪSHT30��ȡ�ĵ�ַ����
        {
            printf( "����ʹ�õ�Ӳ����SHT30_ADDR\r\n");
        }
        break;
        case LIS2DH12_ADDR:
        {
            printf( "����ʹ�õ�Ӳ����LIS2DH12\r\n");
            Lis2dh12_Init();
            filter_calculate(&acc_data, &acc_sample);
            old_angle_calculate(&acc_sample);
        }
        break;
        case BMP280_ADDR:
        {
            printf( "����ʹ�õ�Ӳ����BMP280\r\n");
            bmp280_init();
        }
        break;
        case ADXL345_Addr - 1:
        {
            printf( "����ʹ�õ�Ӳ����ADXL345\r\n");
            Init_ADXL345();
        }
        break;
        case 0x5449:/*FDC2214��16λIIC����ֻ��һ��byte  ������ܽ����� ��Ҫ�ֶ�����0x5449*/
        {
            printf( "����ʹ�õ�Ӳ����FDC2214\r\n");
            FDC2214_Init();
            for(int i = 0; i < 10; i++)
            {
                fdc_res_off[0] += Cap_Calculate(0); //�ɼ����ݣ�ԭʼ���ݣ� ��������
                fdc_res_off[1] += Cap_Calculate(1);
                fdc_res_off[2] += Cap_Calculate(2);
                fdc_res_off[3] += Cap_Calculate(3);
                delay_ms(20);
            }
            fdc_res_off[0] /=10.0;
            fdc_res_off[1] /=10.0;
            fdc_res_off[2] /=10.0;
            fdc_res_off[3] /=10.0;
        
        }
        default:
        {
            printf( "û�в鵽�κ�IIC����!!!!!! %d\r\n", read_id);
//            read_id = 255;
        }
        break;
    }

//    read_id = SHT30_ADDR;//������
    while(1)
    {
        
        switch( read_id )
        {
            case PAJ7620_ID:
            {
                printf( "��� - >%d \r\n",PAJ7620_GetDir());
                delay_ms(100);
            }
            break;
            case SHT30_ADDR+1://��1����ΪSHT30��ȡ�ĵ�ַ����
            {
                int t[20];
                uint16_t h[20];
                SHT30_Read_Humiture(t,h);
                printf("��ʪ��Ϊ%f,%f\n",t[0]/10.0,h[0]/10.0);
                delay_ms(100);
            }
            break;
            case LIS2DH12_ADDR:
            {
                filter_calculate(&acc_data, &acc_sample);
                new_angle_calculate(&acc_sample);
                delay_ms(1000);
            }
            break;
            case BMP280_ADDR:
            {
                printf("�¶�;%0.2f\r\n", bmp280_getTemperature());
                printf("��ѹ;%0.2f\r\n", bmp280_getAtmosPressure());
                printf("�߶�:%0.2f\r\n",bmp280_GetAltitude(bmp280_getTemperature(),bmp280_getAtmosPressure()));
                delay_ms(1000);
            }
            break;
            case ADXL345_Addr - 1:
            {
                Send_ADXL345();
                delay_ms(1000);
            }
            break;
            case 0x5449:/*FDC2214��16λIIC����ֻ��һ��byte*/
            {
                float fdc_res_crr[4];

                fdc_res_crr[0] = fliter(fdc_res_crr[0], Cap_Calculate(0) - fdc_res_off[0]);
                fdc_res_crr[1] = fliter(fdc_res_crr[1], Cap_Calculate(1) - fdc_res_off[1]);
                fdc_res_crr[2] = fliter(fdc_res_crr[2], Cap_Calculate(2) - fdc_res_off[2]);
                fdc_res_crr[3] = fliter(fdc_res_crr[3], Cap_Calculate(3) - fdc_res_off[3]);
                printf("DATA: \r\n ch0: %f, ch1: %f, ch2: %f, ch3: %f \r\n", fdc_res_crr[0], fdc_res_crr[1], fdc_res_crr[2], fdc_res_crr[3]);
                delay_ms(1000);
            }
            break;
            case 0://��ȡ����ID
            {
                if( Ana_I2C_Dev_Add_Scan() != 0 )
                {
                    NVIC_SystemReset(); // ��λ
                }
            }
            break;
            default:
            {
                Ana_I2C_Start();
                Ana_I2C_SendByte(0x5a);
                Ana_I2C_ReceiveAck();
                Ana_I2C_Stop();//����һ��ֹͣ���� 
                delay_ms(1);
                Ana_I2C_Start();
                Ana_I2C_SendByte(0xa5);
                Ana_I2C_ReceiveAck();
                Ana_I2C_Stop();//����һ��ֹͣ���� 
            }
            break;
        }
    }
 }

