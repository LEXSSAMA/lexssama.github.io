---
title: CS144-lab2
index_img: /Picture/CS144.jpg
date: 2021-04-04 18:45:18
tags:
- CS144
- lab2
categories:
- 计算机网络
- CS144
banner_img:
---

# 实现wrapping_integers.cc

**1.wrapping_integers.cc这个文件是干什么的呢？**<br>
wrapping_integers.cc主要做两件事情，一是将64位表示的数包装成32位表示的数。二是将32位的数解包装还原为64位的数。至于为什么要这么做我们下面解释。<br>

**2.为什么要将64位数转化成32位数，32位数还原为64位数呢?**<br>
因为TCP的`Sequence Number`和`Acknowledgement Number`是32位的,最多最多也就只能表示4G大小数据的传输,如果将32位其转化为64位后，就可以表示2^64-1这么大的空间(目前看说几乎是无限大),<br>
*那么怎么转化呢?*<br>
我们用`Initial Sequence Number = 0`,来举例,我们称呼转化后的64位的数字为`Absolute Sequence Number` 。<br>
当TCP传输的数据小于4G的时候(这里假设`Initial Sequence Number = 0`),TCP的`Sequence Number` 与 `Absolute Sequence Number`相同,当TCP传输的数据大于4G后，TCP中的`Sequence Number`又从`0`开始计算，而`Absolute Sequence Number`则等于`Absolute Sequence Number + Sequence Number`。<br>
正是应为如此我们就不知道`TCP`传入的`Sequence Number`，表示的`Absolute Sequence Number`是多少。<br>
举个例子:传入的`Sequence Number`是10,可能表示的`Absolute Sequence Number` 是`{10,2^32+10,2^33+10.....}`,所以我们需要实现wrapping_integers.cc中的函数，将`Sequence Number`还原成正确的`Absoulte Sequence Number`。<br>
同理: 我们要发送`ACK`回去给`Peer`,所以我们也要将`Absolute Sequence Number`转换成正确的32位数发送回去给`Peer`。<br>

```C++
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    uint32_t n32 = n & UINT32_MAX;
    return WrappingInt32{n32 + isn.raw_value()};
}

/*这段是受到清华大佬(huangrt01)博客的启发*/
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint32_t offset = n.raw_value() - wrap(checkpoint, isn).raw_value();
    uint64_t result = checkpoint + offset;
    /*如果新位置距离checkpoint的偏移offset大于1<<32的一半也就是1<<31,
	那么离checkpoint最近的应该是checkpoint前面的元素
    举个例子: 1---------7(checkpoint)----------------1<<32+1;
    由于是无符号数相减所以1-7 == 1<<32+1 - 7;
    所以应该是1距离7最近所以应该选1 
    */
    if (offset > (1u << 31) && result >= (1ul << 32))
        result -= (1ul << 32);
    return result;
}
```

# 实现tcp_receiver.cc

`tcp_receiver`的工作就非常简单了，大概有三个部分:<br>
1. 将接受到的`tcp_segment`塞进lab1实现的`StreamReassembler`中重组字节流，
2. 维护一个`Acknowledgement Number`
3. 维护一个`Window Size` `Window size  = first unacceptable - first unassembled` ;

这一部分比较困难的点在于更新`Acknowledgement Number`，因为传入的`tcp_segment`可能是乱序的，更新`Acknowledgement Number`,就需要知道`StreamReassembler`中一次排序了多少个字节，而且`SYN`和`FIN`分别算一个字节,同样要更新`Acknowledgement Number`。<br>

直接上代码<br>

在tcp_receiver.hh中我的修改如下<br>
```C++
class TCPReceiver {
    size_t _capacity;
    std::optional<WrappingInt32> init_seq_peer;
    std::optional<WrappingInt32> ack_no;
    bool syn;
    bool fin;
	......
}
```

tcp_receiver.cc中<br>
```C++
#include "tcp_receiver.hh"
#include <cstdlib>

void TCPReceiver::segment_received(const TCPSegment &seg) {
    /*只接受第一次的syn所以要满足!init_seq_peer.has_value()*/
    if (seg.header().syn && !init_seq_peer.has_value())
        init_seq_peer = seg.header().seqno;

    if (init_seq_peer.has_value()) {
	/*size_t expected_index = _reassembler.expected_bytes_index();
	 *size_t abs_seq = unwrap(seg.header().seqno,init_seq_peer.value(),expected_index);
	 *_reassembler.push_substring(seg.payload().copy(),abs_seq,seg.header().fin);
	 */
	//上面三句可以合并成这一句
       _reassembler.push_substring(
           seg.payload().copy(),
           unwrap(seg.header().seqno, init_seq_peer.value(), _reassembler.expected_bytes_index()),
           seg.header().fin);

        ack_no = wrap(_reassembler.expected_bytes_index(), init_seq_peer.value());
    }

    fin = fin ? fin : seg.header().fin;

    /*这个判断只会进入一次,(!syn)保证了这个判断只会执行一次*/
    if (!syn && seg.header().syn && ack_no.has_value()) {
	/*这里的代码的主要目的是将Init Sequence Number设置为带Payload的第一个Sequence Number*/
        ack_no = WrappingInt32(ack_no.value().raw_value() + 1);
        init_seq_peer = ack_no;
        syn = true;
    }

    /*只有建立了连接(即init_seq_peer.has_value()为true),
	同时fin来过(即fin==true)，
	而且字符流重组完成(即_reassembler.stream_out().input_ended()返回true)才能为ack_no+1*/
    if (init_seq_peer.has_value() && fin && _reassembler.stream_out().input_ended()) {
        ack_no = WrappingInt32(ack_no.value().raw_value() + 1);
    }
	
    return;
}

optional<WrappingInt32> TCPReceiver::ackno() const { return ack_no; }

size_t TCPReceiver::window_size() const { return this->stream_out().remaining_capacity(); }

```
