---
title: KMP算法
date: 2020-03-22 14:35:33
categories:
- 数据结构
tags:
- 数据结构
---
**感谢:<br>**
[KMP算法 Next数组详解(【洛谷3375】KMP字符串匹配 )](https://blog.csdn.net/qq_30974369/article/details/74276186)<br>
[从头到尾彻底理解 KMP](https://wiki.jikexueyuan.com/project/kmp-algorithm/define.html)<br>
[字符串匹配的KMP算法](http://www.ruanyifeng.com/blog/2013/05/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm.html)<br>



KMP算法即是用来解决在一个字符串S(例如ABCDEFG)中快速查找字符串P(ABCD)的一个算法.<br>
在介绍KMP算法之前我们先介绍暴力查找字符的算法<br>

# 字符串的暴力查找法
如下图用暴力查找法在字符串S(BB....DE)中寻找匹配项字符P(ABCDABD）．<br>


*暴力查找法核心就是发现S[i]和P[j]不相等，S和P就开始回退，S回退到i=i-(j-1)处 ,j回退为０．具体看下图：<br>*


**比较S[0]!=P[0]不相等则回退,i=i-(j-1)=0-0+1=1,j=0,相当于S向前进一步，而P回到j=0再开始比较**
![BL1](KMP算法/BL1.png)
**还是不相等，与上面情况相同**
![BL2](KMP算法/BL2.png)
**S[i]=p[i],i++,j++继续向下比较**
![BL3](KMP算法/BL3.png)
![BL4](KMP算法/BL4.png)
**发现S[i]!=P[j]不相等开始回退**
![BL5](KMP算法/BL5.png)
**置i=i-(j-1)=9-(6-1)=4,j=0,继续比较,即开始比较S[4]=B和P[0]=A,**
![BL6](KMP算法/BL6.png)

可以发现暴力查找的缺点就在发现不相等，S和P都要回退，再重新比较，倘若S和P都特别长，假设S有10000个字符，P有1000个字符，S与P从第０个字符开始相等，而在第998个字符不等，这时，S就要从１号开始再和P从０开始重新比较，太费时．<br>

有没有一种方法让i不改变而只改变j的方法来解决这个字符串的查找问题？没错就是KMP算法.<br>

# KMP算法
在介绍KMP算法之前，需要先介绍**部分匹配值表**<br>

首先，要了解两个概念："前缀"和"后缀"。 "前缀"指除了最后一个字符以外，一个字符串的全部头部组合；"后缀"指除了第一个字符以外，一个字符串的全部尾部组合。

**部分匹配值**就是许多字符串＂前缀＂和＂后缀＂最长的共有元素长度，部分匹配值表就是各个子字符串的所以部分匹配值组成的表．<br>


以字符串ABCDABD为例：<br>
```
-"A",的＂前缀＂和＂后缀＂都是空集　共有元素长度是0． 
-"AB",的＂前缀＂是｛A｝和＂后缀＂是｛B｝　共有元素长度是0 
-"ABC",的＂前缀＂是｛A,AB｝和＂后缀＂是{C,BC}　共有元素长度是0. 
-"ABCD",的＂前缀＂是{A,AB,ABC}和＂后缀＂{D,CD,BCD}　共有元素长度是0 
-"ABCDA",的＂前缀＂是｛A,AB,ABC,ABCD｝和＂后缀＂是{A,DA,CDA,BCDA}　共有元素是A长度是1 
-"ABCDAB",的＂前缀＂是｛A,AB,ABC,ABCD,ABCDA｝和＂后缀＂是｛B,AB,DAB,CDAB,BCDAB｝　共有元素是AB长度是2. 
-"ABCDABD",的＂前缀＂是｛A,AB,ABC,ABCD,ABCDA,ABCDAB｝和＂后缀＂是｛D,BD,ABD,DABD,CDABD,BCDABD,｝　共有元素长度0 
```
|字符串中各个子串|前缀|后缀|最大共有元素长度|
|:-----:|:-----:|:-----:|:-----:|
|A|空集|空集|0|
|AB|｛A｝|｛B｝|0|
|ABC|｛A,AB｝|{C,BC}|0|
|ABCD|{A,AB,ABC}|{D,CD,BCD}　|0|
|ABCDA|｛A,AB,ABC,ABCD｝|{A,DA,CDA,BCDA}|1|
|ABCDAB|｛A,AB,ABC,ABCD,ABCDA｝|｛B,AB,DAB,CDAB,BCDAB｝|2|
|ABCDABD|｛A,AB,ABC,ABCD,ABCDA,ABCDAB｝|｛D,BD,ABD,DABD,CDABD,BCDABD,｝|0|

<br>

如下图KMP算法在遇到S[i]!=P[j]时，i不改变而只改变j，j会向右前进，移动的位数符合以下公式：<br>
```
移动位数（Z）　＝　已匹配位数(X)　－　匹配子字符串的部分匹配值(Y)　
```


![KMP1](KMP算法/KMP举例1.png)<br>

P要向前移动的位数是6-2=4位，再进行比较．
![KMP2](KMP算法/KMP举例2.png)


**为什么移动的位数要等于4呢，如果小于4会怎样？<br>**
答：因为移动的位数等于4，再次比较时P字符前Y个字符不会发生不匹配的现象，而如果移动位数小于4，则再次
字符P,S再次比较时一定会发生不匹配的现象．<br>
**为什么如果右移动小于Z就会发生不匹配的现象呢？**<br>
答：如果向又移动３位则且要求再次比较不会发生不匹配的现象，就要求字符串P的前三位P0P1P2,与P3P4P5相等才不会发生不匹配的现象，但现在现实时P0P1和P4P5相等（部分匹配值为２），P0P1P2和P3P4P5不等，所以向右移动３一定会不匹配，同理向右移动２或者１都一样．<br>

## KMP的代码实现

next[i]里面存放就是计算出来的部分匹配值表的变形<br>
部分匹配值表：<br>
|部分匹配值表|||||||
|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
|0|1|2|3|4|5|6|
|A|AB|ABC|ABCD|ABCDA|ABCDAB|ABCDABD|
|0|0|0|0|1|2|0|

|next[i]表|相当于|部分匹配值|右移１位|再把next[0]|赋值为-1||
|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
|0|1|2|3|4|5|6|
|A|AB|ABC|ABCD|ABCDA|ABCDAB|ABCDABD|
|-1|0|0|0|0|1|2|

**为什么要这么做呢？而且为什么要把next[0]赋值为-1呢？**<br>
因为这样方便写代码．把next[0]赋成-1,我认为是用来做一个标记，如果next[0]=0当两字符串从第0个就不匹配时，向又移动的位数就是0-0=0,相当于不移动，所以把next[0]设置成-1,当j=-1时代表第０个不匹配，就不用公式计算移动位数，直接i++,j++<br>
假设现在文本串 S 匹配到 i 位置，模块串 P 匹配到 j 位置<br>
- 1.if (j == -1 || s[i] == p[j]) 则j++,i++<br>
- 2.否则若s[i] != p[j]&& j!=-1,j=next[j]　(这里就相当于P向右边移动了j-next[j]位)<br>1




``` c

int KmpSearch(char* s, char* p)  
{  
    int i = 0;  
    int j = 0;  
    int sLen = strlen(s);  
    int pLen = strlen(p);  
    while (i < sLen && j < pLen)  
    {  
        //如果j = -1(第0个就不想等直接下一位)，或者当前字符匹配成功（即S[i] == P[j]），都令i++，j++      
        if (j == -1 || s[i] == p[j])  
        {  
            i++;  
            j++;  
        }  
        else  
        {  
            //如果j != -1，且当前字符匹配失败（即S[i] != P[j]），则令 i 不变，j = next[j] (这条指令的效果相当于向右移动Z=j-next[j]位)     
            //next[j]即为j所对应的next值        
            j = next[j];  
        }  
    }  
    if (j == pLen)  
        return i - j;  
    else  
        return -1;  
}  
```
执行上述代码需要的条件是已经计算出需要匹配字符P的next[i]数组．

## next数组的计算

**给出一个字符串P到底怎么计算出next[i]数组呢？**
答：<br>

**这就是给出的字符串，黑黑一坨**<br>
![next1](KMP算法/next1.png)<br> 
**假设:字符串P的红色部分已经匹配，现在准备比较蓝色的位置**<br>
![next2](KMP算法/next2.png)<br>
**蓝色位置和绿色位置比较,(后缀的下一位和前缀的下一位比较)**
![next3](KMP算法/next3.png)<br>
**蓝色与绿色匹配不上,此时寻找红色前缀的最大公共前后缀，即两个灰色部分相等**<br>
![next4](KMP算法/next4.png)<br>
**由于两个红色部分是相同的所以红色后缀，也有两个灰色相同，（这四个灰色都是相同的）**<br>
![next5](KMP算法/next5.png)<br>
**最后用第１个灰色的下一位，也就是紫色，与第４个灰色的下一位，也就是蓝色相比较**<br>
![next6](KMP算法/next6.png)<br>

有点递归的感觉,具体代码如下：<br>
```
inline void GetNext(string s)//获得字符串s的next数组
{
    int l=s.length(),t;
    Next[0]=-1;//如果在0位置失配则是向下移动一位
    for(int i=1;i<l;++i)//依次求解后面的next数组
    {
        t=Next[i-1];
        while(s[t+1]!=s[i]&&t>=0)//循环求解next值 
            t=Next[t];
        if(s[t+1]==s[i])//如果是匹配上而退出循环 
            Next[i]=t+1;
        else            //否则则是匹配不上 
            Next[i]=-1; //指向头 
    }
}
```
