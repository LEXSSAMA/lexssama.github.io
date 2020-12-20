---
title: VIM指令学习
date: 2020-03-22 16:37:18
index_img: /Picture/Vim.jpg
categories:
- vim 
tags:
- vim 
---

# vim 选中的行列递增

**例如想要替换下行中10行BL1,依次递增为BL1.BL2....BL10,可以用这种方法**
```
BL1BL1  
BL1BL1
BL1BL1
BL1BL1
BL1BL1
BL1BL1
BL1BL1
BL1BL1
BL1BL1
BL1BL1
```
先用可视块选中这10行，然后ESC退到命令模式再按：输入命令<br>
```
:'<,'>s/BL\zs\d*\ze/\=line(".")-line("'<")+1/g
```
这些指令的意思如下：
```
'<,'>        我们所选中的区域 (:help '<，:help '> )
s            在选中的区域中进行替换 (:help :s )
\zs          指明匹配由此开始 (:help /\zs )
\d*          查找任意位数的数字 (:help /\d )
\ze          指明匹配到此为止 (:help /\ze )
\=           指明后面是一个表达式 (:help :s\= )
line(".")    当前光标所在行的行号 (:help line() )
line("'<")   我们所选区域中第一行的行号 (:help line() )
/g	     代表一行内所有的BL都使用，如果没有\g就只会对第一个BL使用命令 
```
最后得到的结果是：（有\g）
```
BL1BL1
BL2BL2
BL3BL3
BL4BL4
BL5BL5
BL6BL6
BL7BL7
BL8BL8
BL9BL9
BL10BL10
```
没有/g的情况是：<br>
```
BL1BL1
BL2BL1
BL3BL1
BL4BL1
BL5BL1
BL6BL1
BL7BL1
BL8BL1
BL9BL1
BL10BL1
```
