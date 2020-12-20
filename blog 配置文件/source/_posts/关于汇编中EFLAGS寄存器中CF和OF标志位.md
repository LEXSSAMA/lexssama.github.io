---
title: 关于汇编中EFLAGS寄存器中CF和OF标志位
index_img: /Picture/EFLAGS.jpg
date: 2020-11-15 10:10:31
tags:
- 计算机组成原理
categories:
- 计算机组成原理
banner_img:
---
# 前言
近来在写项目过程中遇到了EFLAGS的OF和CF标志位，这两个功能有点相似，有点搞不清楚，所以去搜索一下，以下的内容是我对回答的粗略的翻译，原回答在此: [about assembly CF(Carry) and OF(Overflow) flag](https://stackoverflow.com/questions/791991/about-assembly-cfcarry-and-ofoverflow-flag)<br>

# 疑问
我知道CF用来解释无符号数的进位，OF解释有符号数的溢出,所以对于有符号数和无符号数都是一串01序列,汇编程序是如何进行区分(differntiate)的？(通过用额外的存储空间记录类型信息?或是通过位置信息？) 还有就是OF和CF两个标志位能否互换(interchangeably)来使用.<br>

# 回答

**有符号数和无符号数的区别在于用什么指令来操作数据，而非数据本身** , 现代计算机(自1970以来)用补码来表示整型数据，所以有符号数和无符号数的加法和减法的其实操作是一样的<br>
- 有符号数和无符号数的不同关键在于如何对最高有效位 (sign bit) 解释，对于有符号数sign bit 用来代表正负(0代表正,1代表负),无符号数则正常计算。
 
- 不同的指令可能对同一个bit的解释不同。
 
- **OF(overflow flag)标志位告诉进位是否将结果最高有效位的符号翻转，以使其与原变量的最高有效位不同,对于无符号数OF标志位没有意义,但是对于有符号数,OF可以表示两数运算是否有溢出(例如:Positive + Positive = Negative)**
 
- **CF(carry flag)标志位用来表示计算时是否存在超过算数逻辑单元长度的进位/借位。<br>
例如:两个8bits的数相加产生超出算数逻辑单元长度的进位,CF被置为1<br>**

|| 11111111 |CF|
| :---: | :----------: | :---: |
| +     | 00000001     | 0     |
| =     | 00000000     | 1     |

**所以对于无符号数来说CF位可以解释为另类的溢出,对于有符号数CF标志位无意义**<br>

