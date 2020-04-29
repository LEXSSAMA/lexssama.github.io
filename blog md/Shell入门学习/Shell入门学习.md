---
title: Shell入门学习
date: 2020-04-09 20:42:58
categories:
- 操作系统
- Shell
tags:
- Shell
---

```
#!/bin/bash
echo -e "hello, world\n"
# 1.变量的定义使用
my_name=lexssama
echo "1.$my_name"
# 2. 另一种定义方式
course="linux start"
echo 2. ${course}
# 3. 只读变量
readonly course
course="linux kernel" #readonly 不能写入(改变)
echo "3. ${course}" # 输出的还是linux start
# 4.删除变量
unset my_name 
echo "4.${my_name}"
# 5.export 导出一个环境变量
export MY_NAME="lexssama"
# 6. 查找自定义的环境变量
env | grep MY_NAME
# 7. 特殊变量
echo "文件名称: $0" # 特殊变量$0表示脚本文件的名称
echo "参数１: $1" #特殊变量$1表示传入第一个参数
echo "参数２: $2" #特殊变量$2表示传入第二个参数
echo "参数３: $3" #特殊变量$3表示传入第三个参数
echo "全部参数: $@" #特殊参数$@表示传入的所有参数
echo "参数个数: $#" #特殊参数$#表示传入参数的个数
# 8. 基本运算
# 8.1 算术运算 (加减乘除取余，+-*/%)
a=16
b=17
# 加法expr 用加法举例子
var0=`expr $a+$b`
echo "$a+$b=$var0"
# 另一种运算方式
var1=$[$a+$b] 
echo "$a+$b=$var1"

#8.2 关系运算
# -eq(相等) -ne(不相等) -gt(大于) -ge(大于等于) -lt(小于) -le(小于等于)
if [ $a -eq $b ] #注意格式是[空格$a.....$b空格]
then
	echo "$a -eq $b : a 等于 b"
else
	echo "$a -eq $b : a 不等于 b"
fi
# 8.3 布尔与逻辑运算
# ! 非运算
# -o 或运算
# -a 与运算
# && 逻辑与 (逻辑与和与运算是不一样的，一个逻辑判断true,false 一个是二进制运算
# || 逻辑或
# == 相等(仅仅限于数字比较)
# != 不相等(仅仅限于数字比较)
if [[ $a -gt 0 && $b -gt 0 ]] #注意格式是[[空格$a.....$b空格]],并且逻辑判断是[[
then 
	echo "a,b都大于0"
fi
# 8.4文件测试运算
# -d 是否为目录
# -f 是否为普通文件
# -r -w -x 是否可读，可写，可执行
# -s 文件是否为空
# -e 文件是否存在
file=$0
if [ -f $file ]
then 
	echo "为普通文件"
fi
if [ -e $file ]
then 
	echo "文件存在"
else
	echo "文件不存在"
fi
if [ -r $file ]
then 
	echo "文件可读"
else
	echo "文件不可读"
fi 
if [ -w $file ]
then 
	echo "文件可写"
else
	echo "文件不可写"
fi
if [ -x $file ]
then 
	echo "文件可执行"
else
	echo "文件不可执行"
fi
# 9.字符串
# 单引号：原样输出，变量无效
# 双引号：可以包含变量
course1="Linux-shell入门学习"
# 单引号
questions='Linux-shell入门学习：$course!'
echo "$questions"
# 双引号
answer="请学习<<$course1>>课程!"
echo "$answer"

# 字符串拼接
echo -e "拼接后一起输出:\n"$questions "\n" $answer
#字符串长度(命令: ${#str})
str="hello,world"
echo "字符串"$str"的长度为:"${#str}
#获取子串，从第一个字符开始截取三个(命令: ${str:1:3})
echo "字符串"$str"子串:"${str:1:3}
#查找子串(命令: `expr index "$str" wo`)
matched=`expr index "$str" wo`
echo "字符串"$str"查找wo的位置在"$matched
# 9.1 字符串运算符号
# = 字符串是否相等　[ $a = $b ]
# != 字符串是否不相等　[ $a != $b ]
# -z 字符串长度是否为0 [ -z $a ]
# -n 字符串长度是否不为0 [ -n "$a" ]
# $　字符串是否为空　[ $a ]


# 10. 数组
# 10.1 数组的定义
arr=("aa" "bb" "cc" "hello world")
# 10.2 设置　元素
arr[2]="222"
# 10.3 读取　元素
echo "下标为2的元素:"${arr[2]}
# 10.4 读取　所有元素
echo "所有元素: "${arr[@]}
# 10.5 获取数组的长度
len=${#arr[@]}
echo "数组长度: $len"
echo "数组长度: "${#arr[@]}

# 11.分支 (if else , case)
# 11.1 if else 

age=20
if [ $age -le 10 ] # <=10
then
	echo "少年"
elif [ $age -le 20 ]  # <=20 注意是elif
then
	echo "青年"
elif [ $age -le 50 ]  # <=50
then
	echo "中年"
else # >50
	echo "老年"
fi

# 11.2 case 
status=1
case $status in
	0) echo "todo" ;;
	1) echo "doing" ;;
	2) echo "done" ;;
esac

# 12.循环
# for...in..do...done
# while...do...done
# util...do...done
# break
for item in ${arr[@]}
do
	echo "$item"
done

# 13. 函数
# function关键字可加可不加 
# 函数后面的()可加可不加
function myfun()
{
	echo "这是shell函数!"
}
myfun
# 函数传参和返回值
function add()
{
	local ret=$(($1+$2))
	return $ret
}
add 5 8
echo $?
```
