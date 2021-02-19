---
title: lvalue and rvalue
index_img: /Picture/lrvalue.png
date: 2021-02-18 21:34:56
tags:
- C++
- Reference
- lvalue rvalue
categories:
- C++
banner_img:
---
如果有小伙伴看到我的文章的话，建议优先去看下面的这些文章,因为我写的质量不及下面文章的十分之一(哭脸)<br>
- [Rvalue References: C++0x Features in VC10, Part 2](https://devblogs.microsoft.com/cppblog/rvalue-references-c0x-features-in-vc10-part-2/)<br>
- [What is move semantics?](https://stackoverflow.com/questions/3106110/what-is-move-semantics)<br>
- [What is the copy-and-swap idiom?](https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom)
- [lvalue VS rvalue, rvalue Reference, and Move Semantics in C++](https://leimao.github.io/blog/CPP-lvalue-rvalue-Reference/)
- [C++笔记 · 右值引用，移动语义，移动构造函数和移动赋值运算符](https://zhuanlan.zhihu.com/p/55229582)

还有这些视频:<br>
- [Move Semantics in C++](https://www.youtube.com/watch?v=ehMg6zvXuMY)
- [Advanced C++: Understanding rvalue and lvalue](https://www.youtube.com/watch?v=UTUdhjzws5g&t=354s)

# lvalue and rvalue 
**在C++中，每一个表达式都是由lvalue和rvalue组成.**<br>

**Example**
```C++
int i = 0;	/*i为lvalue,0为rvalue*/
int a = i+1;		/*a为lvalue,i+1是rvalue*/
1		/*1为rvalue*/
```

*lvalue和rvalue具体的定义是什么?*<br>

实际上lvalue和rvalue很难给出一个具体的定义，但是我们可以给出**简单的,概括的,应对大部分情况的说明。**<br>
- **lvalue :** 一个对象在内存中占据了可识别位置的，有地址的，持久的(脱离了单个表达式语句还依然存在的).
	- 例如: 上面提到的`int a = i+1`,程序运行到了下一条语句`a`依然存在内存中为，`&a`是合法的，lvalue，`i+1`则不存在内存中，不是持久的，`&(i+1)`是不合法的，所以为rvalue。
- **rvalue :** 表达式中任何不是lvalue的对象就是rvalue.

**下面解释出自Lei Mao的博客[lvalue VS rvalue, rvalue Reference, and Move Semantics in C++](https://leimao.github.io/blog/CPP-lvalue-rvalue-Reference/):**<br>
`
lvalue expression is associated with a specific piece of memory, the lifetime of the associated memory is the lifetime of lvalue expression, and we could get the memory address of it. rvalue expression might or might not take memory. Even if an rvalue expression takes memory, the memory taken would be temporary and the program would not usually allow us to get the memory address of it.
`

# Reference 
一个变量的引用可以理解为一个变量的别名,我们可以通过这个别名来实际操作这个变量。<br>
## lvalue Reference (左值引用)
T& 表示lvalue Reference , lvalue Reference能绑定lvalue和rvalue(例外)。<br>

**Example**<br>
```C++
int i = 1;	
int& a = i;		/*a为变量i的lvalue Reference*/
int& b = (a+1)		/*Error! , (a+1)为rvalue*/
a = 2;			/*a==2 ; i==2*/
i = 3;			/*a==3;i==3*/
int const& c = 20;	/*例外:这样可以做到将一个rvalue绑定到一个lvalue reference上*/
```

## rvalue Reference (右值引用)
T&& 用来表示rvalue Reference , rvalue Reference只能绑定rvalue。<br>
**Example**<br>
```C++
int a = 1;
int && b = a+1;	/*b为rvalue Reference*/
```

rvalue Reference 常常用于 **Move Semantics** 和 **Perfect Forward**。

### Move Semantics
我们先引入一段代码再做介绍:<br>
```C++
#include <stdio.h>
#include <algorithm>
#include <cstring>
class String
{
    char* data;

public:

	/*Command Constructor*/
    String(const char* p)
    {
	printf("Command Constructor!\n");
        size_t size = std::strlen(p) + 1;
        data = new char[size];
        std::memcpy(data, p, size);
    }
   ~String()
    {
	printf("Destoryed!\n");
        delete[] data;
    }
	/*Copy Constructor*/
    String(const String& that)
    {
	printf("Copy Constructor!\n");
        size_t size = std::strlen(that.data) + 1;
        data = new char[size];
        std::memcpy(data, that.data, size);
    }
	/*Move Constructor*/
    String(String&& that)   // string&& is an rvalue reference to a string
    {
	printf("Move Constructor!\n");
        data = that.data;
        that.data = nullptr;
    }
	char* getData(){
		return data;
	}
	void printfString(){
		printf("%s\n",data);
	}
};

class student {
	private:
		String name;
	public:
		student(const String& s):name(s){}
		//student(String&& s):name((String&&) s){}
	void printfName(){
		name.printfString();
	}
};

int main(){
	student a("LiQiBin");
	a.printfName();
	return 0;
}
```
通常情况下我们想创建一个`student`类，我们调用函数`student a("LiQiBin")`,函数首先会调用`String`类中的`Command Constructor`来创建一个传入参数，传入参数为`lvalue Reference`，虽然`LiQiBin`是一个`rvalue`但是会被转化为`lvalue Reference`，参照上文`lvalue Reference`的介绍,然后当`student`的构造器执行到`name(s)`时，程序又会调用`String`类中的`Copy Constructor`，最后才得到完整的`student`参数。

**输出如下:**<br>
```C++
Command Constructor!
Copy Constructor!
Destoryed!
LiQiBin
Destoryed!
```
这个过程中分别调用了`String`类的`Command Constructor`和`Copy Constructor`这就意味着执行了两次内存的分配(`new`操作)和复制`memcpy()`操作,`String`类中的字符数非常少还可以接受，如果被操作的`String`类内的字符数量极其庞大,两次内存的分配和复制就是对资源相当大的浪费! 是否有办法，只执行一次复制就可以构造出student类呢?<br>
答案是右值引用(rvalue Reference)的**Move Semantics**<br>

我们取消代码中的`student(String&& s):name((String&&) s){}`这行注释，这样rvalue`LiQiBin`就会优先被解释为`rvalue Reference`,程序就会先执行`String`类中的`Command Constructor`构造出一个传入参数,然后执行`name((String&&)s)`,即执行`String`类中的`Move Constructor`。<br>

**下面是Move Constructor的代码：**<br>
```C++
	/*Move Constructor*/
    String(String&& that)   // string&& is an rvalue reference to a string
    {
	printf("Move Constructor!\n");
        data = that.data;
        that.data = nullptr;
    }
```
可以看到`Move Constructor`没有调用`new`和`memcpy`而只是将`rvalue Reference`中的`that.data`指向的地址**偷来**给自己用，再将`that.data`指向`nullptr`,因为`rvalue`是暂时的，不持久的参数，`student a("LiQiBin")`执行结束后生成的临时`String`类就会调用析构函数将得到的资源收回，所以将`that.data`指向`nullptr`就避免了,临时`String`类收回资源时将指向`LiQiBin`位置收回，**就相当于`Move Constructor`将`rvalue Reference`中的资源偷来**,成功只用一次`new`和`memcpy`就构建出了`student`类。<br>

**输出如下:**<br>
```C++
Command Constructor!
Move Constructor!
Destoryed!
LiQiBin
Destoryed!
```


