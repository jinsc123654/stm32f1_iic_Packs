#include "stdio.h"
#include "ANALOG_I2C.h"
#include "delay.h"
#include "PAJ7620.h"
                          
/**
  * @brief PAJ7620选择BANK区
  * @param bank 选择的区的编号
  * @retval 无
  */
void PAJ7620_Select_Bank(unsigned char bank)
{
    switch (bank)
    {
    case 0:
        Ana_I2C_E_W(PAJ7620_ID, PAJ_REGITER_BANK_SEL, PAJ_BANK0);
        break;
    case 1:
        Ana_I2C_E_W(PAJ7620_ID, PAJ_REGITER_BANK_SEL, PAJ_BANK1);
        break;
    }
}

/**
  * @brief PAJ7620唤醒函数
  * @param 无
  * @retval 唤醒成功返回1,否则为0
  */
unsigned char PAJ7620_WakeUp(void)
{
    unsigned char data = 0x0a;
    Ana_I2C_Start();
    Ana_I2C_SendByte(PAJ7620_ID);
    Ana_I2C_Stop();
    delay_us(5);
    PAJ7620_Select_Bank(0);
    data = Ana_I2C_E_R(PAJ7620_ID, 0x00);
    if (data != 0x20)
        return 0;
    return 1;
}

/**
  * @brief PAJ7620初始化
  * @param 无
  * @retval 初始化成功返回1,否则为0
  */
unsigned char PAJ7620_Init(void)
{
    unsigned char i, status;
    status = PAJ7620_WakeUp();
    if (!status)
    {
        printf( "唤醒PAJ7620 失败\r\n" );
        return 0;
    }
    else
    {
        printf( "唤醒PAJ7620 成功\r\n" );
    }

    PAJ7620_Select_Bank(0);
    for (i = 0; i < INIT_SIZE; i++)
    {
        Ana_I2C_E_W(PAJ7620_ID, init_Array[i][0], init_Array[i][1]);
    }
    PAJ7620_Select_Bank(0);
    Ana_I2C_E_W(PAJ7620_ID, PAJ_SET_INT_FLAG1, 0xFF);
    return 1;
}

/**
  * @brief PAJ7620读取手势标识
  * @param 无
  * @retval 返回手势标识
  */
unsigned char PAJ7620_GetDir(void)
{
    unsigned char res = 0;
    switch (Ana_I2C_E_R(PAJ7620_ID, PAJ_GET_INT_FLAG1))
    {
        case 0x01:
            res = 1;
            break;
        case 0x02:
            res = 2;
            break;
        case 0x04:
            res = 3;
            break;
        case 0x08:
            res = 4;
            break;
        case 0x10:
            res = 5;
            break;
        case 0x20:
            res = 6;
            break;
        case 0x40:
            res = 7;
            break;
        case 0x80:
            res = 8;
            break;
        default:
            break;
    }

    return res;
}
