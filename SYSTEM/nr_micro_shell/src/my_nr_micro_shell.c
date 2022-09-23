/*
 * @Author: Jinsc
 * @Date: 2022-09-23 09:10:59
 * @LastEditors: Jinsc
 * @LastEditTime: 2022-09-23 09:54:28
 * @FilePath: \IIC_多器件例程\SYSTEM\nr_micro_shell\src\my_nr_micro_shell.c
 * @Description: 
 * Copyright (c) 2022 by jinsc123654@gmail.com All Rights Reserved. 
 */
#include "../inc/my_nr_micro_shell.h"
#include "string.h"
//#include "malloc.h"

#define RINGBUFF_LEN 512 /* 定义的环形缓存区大小 */
typedef struct
{
    uint16_t Head;           
    uint16_t Tail;
    uint16_t Lenght;
    uint8_t Ring_Buff[RINGBUFF_LEN];
}RingBuff_t;
static RingBuff_t ringBuff;//创建一个ringBuff的缓冲区

static void RingBuff_Init(void)//初始化环形缓冲区
{
   //初始化相关信息
   ringBuff.Head = 0;
   ringBuff.Tail = 0;
   ringBuff.Lenght = 0;
}
 
 /**
  * @brief  往环形缓冲区写入uint8_t类型的数据 该函数就是shell获取字符串的地方
  * @param  无
  * @retval 无
  */
int8_t Write_RingBuff(uint8_t data)//
{
   if(ringBuff.Lenght >= RINGBUFF_LEN) //判断缓冲区是否已满
    {
      return -1;
    }
    ringBuff.Ring_Buff[ringBuff.Tail]=data;
//    ringBuff.Tail++;
    ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//防止越界非法访问
    ringBuff.Lenght++;
    return 0;
}
 /**
  * @brief  从环形缓冲区读取uint8_t类型的数据 该函数就是shell处理数据的地方
  * @param  无
  * @retval 无
  */
static int8_t Read_RingBuff(uint8_t *rData)
{
   if(ringBuff.Lenght == 0)//判断非空
    {
       return -1;
    }
   *rData = ringBuff.Ring_Buff[ringBuff.Head];//先进先出FIFO，从缓冲区头出
//   ringBuff.Head++;
   ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//防止越界非法访问
   ringBuff.Lenght--;
   return 0;
}
 /**
  * @brief  shell发送一个字符
  * @param  无
  * @retval 无
  */
void shell_putchar(char temp)
{
    USART1->DR=temp;
    while((USART1->SR&0X40)==0);//等待发送结束
}

 /**
  * @brief  初始化环形缓存区以及shell组件
  * @param  无
  * @retval 无
  */
void my_shell_init(void)
{
    RingBuff_Init();//初始化环形队列
    shell_init();
}
 /**
  * @brief  从环形缓存区读取一个字符并用shell处理
  * @param  无
  * @retval 无
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
  * @brief  注册一个ls命令
  * @param  无
  * @retval 无
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
            shell_printf("项目编译时间为: ");
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
    __set_FAULTMASK(1); //关闭所有中断
    NVIC_SystemReset(); //进行软件复位
  }
}
NR_SHELL_CMD_EXPORT(reset, reset_cmd);

//void free_mem(char argc, char *argv)    
//{
//    printf("总内存  ->%d\n", (size_t)mem_size_aligned);
//    printf("使用内存->%d\n", (size_t)used_mem);
//    printf("最高使用->%d\n", (size_t)max_mem);
//}
//NR_SHELL_CMD_EXPORT(free_mem, free_mem);


 /**
  * @brief  注册一个命令有两种模式，一种是分散加载 另一种是手动添加数组
  * @param  无
  * @retval 无
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
