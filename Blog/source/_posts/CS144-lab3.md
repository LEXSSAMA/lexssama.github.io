---
title: CS144-lab3
index_img: /Picture/CS144.jpg
date: 2021-04-08 15:29:29
tags:
- CS144
- lab3
categories:
- 计算机网络
- CS144
banner_img:
---

# 实现TCPSender
这个lab有几个难点,分别是:<br>
1. TCP中会维护好几种状态:
	- CLOSE: SYN还没有被发送
	- SYN_SENT: SYN被发送了但是没有收到对应的ACK
	- SYN_ACKED: 正常状态，可以利用sender来通信了
	- SYN_ACKED_ASLO: sender已经发送完所以的字节流了，但是还没有发生FIN。
	- FIN_SENT: FIN已经被发送，但是还没有收到对应的ACK
	- FIN_ACKED: sender已经完成所以的任务。

2. TCPSender使用的是`累计确认`的协议，也就是说如果收到`合法的ACK`,那么ACK之前的所有已经发送segments已经被成功接受。(如果收到的ACK大于我还未发送的Sequence Number那就是非法的ACK)
 
3. timer的开启，重启，关闭的时机。<br>
在做这个是lab之前我一直以为是给每一个发出的segments安装一个timer，当segments超时的时候重发，但是看了`huangrt01`大佬的代码后，发现在这个lab中timer,timer记录的是在RTO时间内有没有收到`合法的ACK`,如果没有收到`合法的ACK`，那么就需要重传。<br>
**下面简单的概括一下timer使用的时机:**<br>
	- 当发送一个新的segment的时候，如果timer没有开启，那么需要开启timer。
	- 当在RTO内收到一个`合法的ACK`,有两种情况: 
		1. 如果sender没发完segments那么需要`重启timer`,重启的意思是timer从0开始计时。
		2. 如果sender已经发完所有的segments了那么需要`关闭timer`
	- 当超时的情况发生,也是两种情况:<br>
		1. window_size = 0 : `重启timer`,`重传segments`。
		2. window_size != 0 : `double RTO`, `重启timer`,`重传segments`。

**timer实现更加规范看[RFC 6298](https://datatracker.ietf.org/doc/rfc6298/?include_text=1)第五小节**
# 代码

**tcp_sender.hh**<br>
```c++
//! \make by myself
class TCPTimer {
  private:
    //! true : timer start , false : timer not start
    bool _start;
    unsigned int init_time;
    //! Transmission time
    unsigned int transmission_time;
    //! retransmission timeout
    unsigned int RTO;

  public:
    //! Number of consecutive retransmissions
    unsigned int num_of_retransmission;
	
    TCPTimer(unsigned int time)
        : _start(false)
		, init_time(time)
		, transmission_time(0)
		, RTO(init_time)
		, num_of_retransmission(0) {}

    bool running() { return _start; }

    void close() {
        _start = false;
        num_of_retransmission = 0;
    }
    void start() {
        _start = true;
        RTO = init_time;
        transmission_time = 0;
        num_of_retransmission = 0;
    }
//! if window == 0 then keep RTO , otherwise double RTO
    void doubleOrkeep_RTO_and_restart(const size_t window) {
        if (!running())
            return;

        if (window != 0)
            RTO *= 2;

        transmission_time = 0;
        num_of_retransmission++;
    }

    bool timeout(const size_t ms_since_last_tick) {
        if (!running())
            return false;

        if (ms_since_last_tick + transmission_time >= RTO)
            return true;

        transmission_time += ms_since_last_tick;
        return false;
    }
};

class TCPSender {
  private:
   .............
    //! make by myself
    uint64_t _ackno;
    size_t _window_size;
    uint64_t _bytes_in_flight;
    TCPTimer timer;

    std::queue<TCPSegment> _segments_track{};

    void send_no_empty_segments(TCPSegment &seg);
    
    .............
}
```

**tcp_sender.cc**<br>
```C++
#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout
			,const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , _ackno(0)
    , _window_size(1)
    , _bytes_in_flight(0)
    , timer(retx_timeout) {}

uint64_t TCPSender::bytes_in_flight() const { return _bytes_in_flight; }

void TCPSender::send_no_empty_segments(TCPSegment &seg) {
    seg.header().seqno = wrap(_next_seqno, _isn);
    _next_seqno += seg.length_in_sequence_space();
    _bytes_in_flight += seg.length_in_sequence_space();
    _segments_out.push(seg);
    _segments_track.push(seg);
    if (!timer.running()) {
        timer.start();
    }
}

void TCPSender::fill_window() {
    /*Status: CLOSED -> stream waiting to begin*/
    if (_next_seqno == 0) {
        TCPSegment seg;
        seg.header().syn = true;
        seg.header().seqno = next_seqno();
        send_no_empty_segments(seg);
    }
    /*Status: SYN_SENT -> stream start but nothing acknowledged*/
    else if (_next_seqno == _bytes_in_flight) {
        return;
    }
	
    size_t window_size = _window_size == 0 ? 1 : _window_size;
    size_t remain = 0;
//! 这里window_size 一定是大于 (_next_seqno - _ackno),不用担心溢出问题。文章后面解释
	while ((remain = window_size - (_next_seqno - _ackno))) {
        TCPSegment seg;
size_t len = TCPConfig::MAX_PAYLOAD_SIZE > remain ? remain : TCPConfig::MAX_PAYLOAD_SIZE;
        /*Status: SYN_ACKED -> stream ongoing*/
        if (!_stream.eof()) {
            seg.payload() = Buffer(_stream.read(len));
            if (_stream.eof() && remain - seg.length_in_sequence_space() > 0)
                seg.header().fin = true;
            if (seg.length_in_sequence_space() == 0)
                return;
            send_no_empty_segments(seg);
        }
/*Status: SYN_ACKED -> stream ongoing (stream has reached EOF but FIN hasn't been send yet)*/
        else if (_stream.eof()) {
            if (_next_seqno < _stream.bytes_written() + 2) {
                seg.header().fin = true;
                send_no_empty_segments(seg);
            }
         /*Status: FIN_SENT and FIN_ACKED both do nothing Just return */
            else
                return;
        }
    }
}

void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t abs_ackno = unwrap(ackno, _isn, _ackno);

    //! 超出范围 _next_seqno还没发呢，哪来的abs_ackno > _next_seqno
    if (abs_ackno > _next_seqno) {
        return;
    }

    _window_size = static_cast<size_t>(window_size);

    //! 比abs_ackno大的先来了
    if (abs_ackno <= _ackno) {
        return;
    }

    _ackno = abs_ackno;

    //! 成功接受到新的ackno
    timer.start();

    while (!_segments_track.empty()) {
        TCPSegment seg = _segments_track.front();
        if (ackno.raw_value() < seg.header().seqno.raw_value() 
		+ static_cast<uint32_t>(seg.length_in_sequence_space()))
            break;
        _bytes_in_flight -= seg.length_in_sequence_space();
        _segments_track.pop();
    }

    fill_window();

    if (_segments_track.empty()) {
        timer.close();
    }
}

void TCPSender::tick(const size_t ms_since_last_tick) {
    if (!timer.running() || !timer.timeout(ms_since_last_tick))
        return;
    if (_segments_track.empty()) {
        timer.close();
        return;
    }
    timer.doubleOrkeep_RTO_and_restart(_window_size);
    _segments_out.push(_segments_track.front());
}

unsigned int TCPSender::consecutive_retransmissions() const { 
	return timer.num_of_retransmission; 
}

void TCPSender::send_empty_segment() {
    TCPSegment seg;
    seg.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(seg);
} 
```

**解释remain = window_size - (_next_seqno - _ackno))不会溢出**<br>
要解释这里就要联系一下lab2中实现的TCPReceiver<br>
这里的window_size指的是receiver中buffer剩余的空间,当receiver往buffer中放入字节流的时候window_size变小，当上层应用读取buffer中的字节流的时候，window_size变大。<br>
**注意:**
不是sender发送segment给receiver,receiver就立刻将segement放入buffer中了,而是当segments连续了才会被放入buffer中<br>

这里我假设上次发送时的窗口大小为window_size_pre,同时上次 **发送的首个segments就没有被receiver接收到** ,而其他的都被接收到了, **这个时候receiver不会把这批segments放入buffer中** ，所以发回来的ACK中装的window_size是等于window_size_pre的，所以window_size 不会大于(_next_seqno - _ackno);
