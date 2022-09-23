/*
 * @Author: Jinsc
 * @Date: 2022-09-23 09:10:59
 * @LastEditors: Jinsc
 * @LastEditTime: 2022-09-23 09:54:28
 * @FilePath: \IIC_����������\SYSTEM\nr_micro_shell\src\my_nr_micro_shell.c
 * @Description: 
 * Copyright (c) 2022 by jinsc123654@gmail.com All Rights Reserved. 
 */
#include "../inc/my_nr_micro_shell.h"
#include "string.h"
//#include "malloc.h"

#define RINGBUFF_LEN 512 /* ����Ļ��λ�������С */
typedef struct
{
    uint16_t Head;           
    uint16_t Tail;
    uint16_t Lenght;
    uint8_t Ring_Buff[RINGBUFF_LEN];
}RingBuff_t;
static RingBuff_t ringBuff;//����һ��ringBuff�Ļ�����

static void RingBuff_Init(void)//��ʼ�����λ�����
{
   //��ʼ�������Ϣ
   ringBuff.Head = 0;
   ringBuff.Tail = 0;
   ringBuff.Lenght = 0;
}
 
 /**
  * @brief  �����λ�����д��uint8_t���͵����� �ú�������shell��ȡ�ַ����ĵط�
  * @param  ��
  * @retval ��
  */
int8_t Write_RingBuff(uint8_t data)//
{
   if(ringBuff.Lenght >= RINGBUFF_LEN) //�жϻ������Ƿ�����
    {
      return -1;
    }
    ringBuff.Ring_Buff[ringBuff.Tail]=data;
//    ringBuff.Tail++;
    ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
    ringBuff.Lenght++;
    return 0;
}
 /**
  * @brief  �ӻ��λ�������ȡuint8_t���͵����� �ú�������shell�������ݵĵط�
  * @param  ��
  * @retval ��
  */
static int8_t Read_RingBuff(uint8_t *rData)
{
   if(ringBuff.Lenght == 0)//�жϷǿ�
    {
       return -1;
    }
   *rData = ringBuff.Ring_Buff[ringBuff.Head];//�Ƚ��ȳ�FIFO���ӻ�����ͷ��
//   ringBuff.Head++;
   ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
   ringBuff.Lenght--;
   return 0;
}
 /**
  * @brief  shell����һ���ַ�
  * @param  ��
  * @retval ��
  */
void shell_putchar(char temp)
{
    USART1->DR=temp;
    while((USART1->SR&0X40)==0);//�ȴ����ͽ���
}

 /**
  * @brief  ��ʼ�����λ������Լ�shell���
  * @param  ��
  * @retval ��
  */
void my_shell_init(void)
{
    RingBuff_Init();//��ʼ�����ζ���
    shell_init();
}
 /**
  * @brief  �ӻ��λ�������ȡһ���ַ�����shell����
  * @param  ��
  * @retval ��
  */
void my_shell_run(void)
{
    char c;
    while( Read_RingBuff((uint8_t*)&c) == 0 )
    {
        shell(c);
    }
}
 /**
  * @brief  ע��һ��ls����
  * @param  ��
  * @retval ��
  */
static void shell_ls_cmd(char argc, char *argv)
{
    unsigned int i = 0;
    if (argc > 1)
    {
        if (!strcmp("-cmd", &argv[argv[1]]))
        {

            for (i = 0; nr_shell.static_cmd[i].fp != NULL; i++)
            {
                shell_printf("%s",nr_shell.static_cmd[i].cmd);
                shell_printf("\r\n");
            }
        }
        else if (!strcmp("-v", &argv[argv[1]]))
        {
            shell_printf("ls version 1.0.\r\n");
            shell_printf("��Ŀ����ʱ��Ϊ: ");
            shell_printf(__TIME__);
            shell_printf(" ");
            shell_printf(__DATE__);
            shell_printf("\r\n");
        }
        else if (!strcmp("-h", &argv[argv[1]]))
        {
            shell_printf("useage: ls [options]\r\n");
            shell_printf("options: \r\n");
            shell_printf("\t -h \t: show help\r\n");
            shell_printf("\t -v \t: show version\r\n");
            shell_printf("\t -cmd \t: show all commands\r\n");
        }
    }
    else
    {
        shell_printf("ls need more arguments!\r\n");
    }
}
void shell_test_cmd(char argc, char *argv)
{
    unsigned int i;
    shell_printf("test command:\r\n");
    for (i = 0; i < argc; i++)
    {
        shell_printf("paras %d: %s\r\n", i, &(argv[argv[i]]));
    }
}
void reset_cmd(char argc, char *argv)    
{
    while (1)
  {
    __set_FAULTMASK(1); //�ر������ж�
    NVIC_SystemReset(); //���������λ
  }
}
NR_SHELL_CMD_EXPORT(reset, reset_cmd);

//void free_mem(char argc, char *argv)    
//{
//    printf("���ڴ�  ->%d\n", (size_t)mem_size_aligned);
//    printf("ʹ���ڴ�->%d\n", (size_t)used_mem);
//    printf("���ʹ��->%d\n", (size_t)max_mem);
//}
//NR_SHELL_CMD_EXPORT(free_mem, free_mem);


 /**
  * @brief  ע��һ������������ģʽ��һ���Ƿ�ɢ���� ��һ�����ֶ��������
  * @param  ��
  * @retval ��
  */
#ifdef NR_SHELL_USING_EXPORT_CMD
NR_SHELL_CMD_EXPORT(ls, shell_ls_cmd);
NR_SHELL_CMD_EXPORT(test, shell_test_cmd);
#else
const static_cmd_st static_cmd[] =
    {
        {"ls", shell_ls_cmd},
        {"test", shell_test_cmd},
        {"\0", NULL}};
#endif
