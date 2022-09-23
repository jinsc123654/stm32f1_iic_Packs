/*
 * @Descripttion: 文件描述
 * @version: 文件版本
 * @Author: jinsc
 * @Date: 2022-07-28 09:03:33
 * @LastEditors: jinsc
 * @LastEditTime: 2022-07-28 16:47:24
 * @FilePath: \1_HW_test\User\packages\nr_micro_shell\inc\my_nr_micro_shell.h
 */
#ifndef __my_nr_micro_shell_h
#define __my_nr_micro_shell_h
#include "./nr_micro_shell.h" 
#include "stm32f10x.h"
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>

void my_shell_init(void);//初始化shell
void my_shell_run(void);//处理命令的函数
int8_t Write_RingBuff(uint8_t data);//往环形缓冲区写入u8类型的数据
#endif /*endif __my_nr_micro_shell_h*/
