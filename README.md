<!--
 * @Descripttion: 文件描述
 * @version: 文件版本
 * @Author: jinsc
 * @Date: 2022-06-02 15:51:15
 * @LastEditors: jinsc
 * @LastEditTime: 2022-08-23 13:37:55
 * @FilePath: \undefinedd:\DSP\stm32f103ve\1_HW_test\README.md
-->
# 本版本库是stm32f1验证代码

# shell说明（可以根据my_nr_micro_shell.c设置自己的shell）

1：输入 iic_scan 回车可以完成iic扫描（暂时值支持8位地址）

2：输入 reset 回车可以完成复位单片机

3：输入 ls -cmd 回车可以查看支持的shell

# 已经验证完毕的硬件
1：ADXL345

2:BMP280

3:FDC2214（这个是16位IIC 需要手动指定才能进入）

4:LIS2DH12

5:PAJ7620

6:SHT30