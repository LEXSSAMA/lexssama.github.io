---
title: 52. N-Queens II
index_img: /Picture/N_Queens.jpg
date: 2020-11-14 23:18:32
tags:
- 力扣
categories:
- 力扣
banner_img:
---
# N皇后
N皇后是经典题目了，这篇文章不写解题思路,看解题思路可以看这一篇力扣的官方文章[52. N-Queens](https://leetcode-cn.com/problems/n-queens-ii/solution/nhuang-hou-ii-by-leetcode-solution/)
**这篇文章主要记录一下用二进制数解N皇后的疑问和解答**<br>

先贴代码:
```java
class Solution {
    public int totalNQueens(int n) {
        return solve(n, 0, 0, 0, 0);
    }
    public int solve(int n, int row, int columns, int diagonals1, int diagonals2) {
        if(n==row)
            return 1;
        int count =0;
        int availablePosition = ~(columns|diagonals1|diagonals2)&((1<<n)-1);
        while(availablePosition>0)
        {
            int position = availablePosition&(-availablePosition);
            count += solve(n,row+1,columns|position,(diagonals1|position)<<1,(diagonals2|position)>>1);
            availablePosition &=(availablePosition-1);
        }
        return count ;
    }
}
```
## 疑问
**疑问一: int position = availablePosition&(-availablePosition);这段代码的作用是什么?**<br>
二进制位运算:正数x与其相反数-x的与操作x&(-x)得到的数是二进制x第一个出现的1代表的值(这里说的不太清楚看下面例子)<br>

| 注意：负数在计算机中用补码表示 |  二进制  | 十进制 |
|:------------------------------:|:--------:|:------:|
|                x               | 01100100 |   100  |
|               -x               | 10011100 |  -100  |
|             x&(-x)             | 00000100 |    4   |


由上表可以看出我们可以通过x&(-x)提取出x的二进制表示时第一个不为0的数.

**疑问二: availablePosition &=(availablePosition-1);这段代码的作用是?**<br>

我们把availablePosition 看成x，这样好表示一点.<br>
x&=(x-1)是为了将x二进制中第一个不为0的数置为0,看下面例子<br>

| -        | 二进制   | 十进制       |
|----------|----------|--------------|
| x        | 01100100 | 100          |
| x-1      | 01100010 | 99           |
| x&=(x-1) | 01100000 | 这个数不重要 |

通过上表可以看到x&(x-1)成功把x中第一个不为0的数置为0.
