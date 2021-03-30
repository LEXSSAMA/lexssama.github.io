---
title: CS144-lab0
index_img: /Picture/wallhaven-q6q2e5.jpg
date: 2021-03-27 16:32:07
tags:
- CS144 
- lab0
categories:
- 计算机网络
- CS144 
banner_img:
---
# 实现webget.cc

实现webget.cc实际上就是补充`get_URL()`函数。<br>
**这里比较值得注意的是通过套接字与采用http协议的服务器沟通的语法是这样的:**<br>
```http
GET /hello HTTP/1.1		//注意这里换行符号为\r\n
Host: cs144.keithw.org
Connection:  close
```
在程序上表现为:<br>
`"GET " + path + " " + "HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n"`<br>

```C++
void get_URL(const string &host, const string &path) {

    TCPSocket sock1;
    Address addr = Address(host, "http");
    sock1.connect(addr);
    sock1.write("GET " + path + " " + "HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n");
    while (1) {
        auto recv = sock1.read(RECV_SIZE);
        cout << recv;
        if (sock1.eof())
            break;
    }
    sock1.close();
}
```

# 实现byte_stream.hh

实现byte_stream.hh即是实现byteStream这个类内部的一些函数，达到实现内存中可靠字节流的任务。<br>

什么叫做内存中可靠字节流 \<An in-memory reliable byte stream\>呢?<br>

其实就是将数据流写入一个buffer中然后再读出来，由于是单线程操作，所以这个实现也比较简单。

使用数据结构`std::deque`即完成。<br>


## 坑点1

实现文件`EOF`的时候，并不是文件读完就设置`EOF`为`true`。<br>

那什么时候设置`EOF`为`True`呢?<br>

答案是: 当程序调用`end_input()`的时候，这个时候程序表示输入已经完成,在这个之后的读出`buffer_size==0`的时候才可以将`EOF`设置为`true`。


## 坑点2

一开始我并没有使用`std::deque`，而是用一个`char* Stream`在构造函数中使用`new`得到空间，存储数据,但是给出的byteStream类中没有析构函数，所以为了不将byteStream类改变太大，我决定使用`std:;deque`。

