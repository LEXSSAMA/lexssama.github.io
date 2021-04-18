---
title: CS144-lab5
index_img: /Picture/CS144.jpg
date: 2021-04-19 00:36:43
tags:
- CS144
- lab4
categories:
- 计算机网络
- CS144
banner_img:
---

# 实现Network Interface
网络接口`Network Interface`是网络层和链路层链接的桥梁，它的作用就是将网络层传下来的`Internet datagrams`封装成链路层的`Ethernet frames`,然后将封装好的`Ethernet frames`发出。<br>

## 封装Ethernet frames

想将`Internet datagrams`封装成`Ethernet frames`我们需要知道`Destination Mac Address`和`Source Mac Address` 和 `EtherType`。

- **Source Mac Address** : 本机的Mac地址。
- **Destination Mac Address** : 目标主机的Mac地址会被存储到本机的目标IP和目标Mac地址的映射数据结构中,如果本机维护的这个数据结构不存在目标IP的Mac地址，那么我们就需要使用ARP协议向局域网中的其他主机询问目标IP的Mac地址。(APR协议的细节:[Address Resolution Protocol](https://en.wikipedia.org/wiki/Address_Resolution_Protocol))
- **EtherType** : 表示上层`Internet datagrames`使用的协议类型，lab5中只涉及到`ARP`和`IPv4`协议。

## 细节
1. 因为目标Mac地址缺失而没发出的`Ethernet frames`，得到目标Mac地址后需要重发。
2. 不能在`5秒内`向使用`ARP Request` 询问同一个目标IP的Mac地址。
3. 每一个存储在数据结构中的目标ip-mac映射对过了`30秒`后需要被清理。

## 代码
**network_interface.hh**<br>
```c++
class NetworkInterface {
  private:
....
    struct Dgram_nextHop {
        const InternetDatagram dgram;
        Address next_hop;
    };
    struct EtherAddress_Time {
        EthernetAddress mac_addr;
        size_t time;
    };

    std::queue<Dgram_nextHop> _frames_track{};

    std::map<uint32_t, EtherAddress_Time> _ip_eth_map{};

    std::map<uint32_t, size_t> _ip_msTime_map{};

    size_t _ms_clock{0};

    void send_arp(uint16_t opcode, EthernetAddress &dst, uint32_t dst_ip);
....
}
```

**network_interface.cc**<br>
```c++
#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)

bool NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();

    if (_ip_eth_map.count(next_hop_ip)) {
        if (_ms_clock - _ip_eth_map[next_hop_ip].time >= 30 * 1000) {
            _ip_eth_map.erase(next_hop_ip);
            _frames_track.push(Dgram_nextHop{dgram, next_hop});
            send_arp(ARPMessage::OPCODE_REQUEST, const_cast<EthernetAddress &>(ETHERNET_BROADCAST), next_hop_ip);
            _ip_msTime_map[next_hop_ip];
            return false;
        }

        EthernetFrame frame;
        frame.header().src = _ethernet_address;
        frame.header().dst = _ip_eth_map[next_hop_ip].mac_addr;
        frame.header().type = EthernetHeader::TYPE_IPv4;

        frame.payload() = std::move(dgram.serialize());
        _frames_out.push(std::move(frame));
        return true;
    }
    if (!_ip_msTime_map.count(next_hop_ip)) {
        _frames_track.push(Dgram_nextHop{dgram, next_hop});
        send_arp(ARPMessage::OPCODE_REQUEST, const_cast<EthernetAddress &>(ETHERNET_BROADCAST), next_hop_ip);
        _ip_msTime_map[next_hop_ip];
    } else if (_ms_clock - _ip_msTime_map[next_hop_ip] >= 5 * 1000) {
        send_arp(ARPMessage::OPCODE_REQUEST, const_cast<EthernetAddress &>(ETHERNET_BROADCAST), next_hop_ip);
        _ip_msTime_map.erase(next_hop_ip);
    }
    return false;
}

void NetworkInterface::send_arp(uint16_t opcode, EthernetAddress &dst, uint32_t dst_ip) {
    ARPMessage arp;
    EthernetFrame frame;
    arp.opcode = opcode;
    arp.sender_ethernet_address = _ethernet_address;
    arp.sender_ip_address = _ip_address.ipv4_numeric();
    if (opcode != ARPMessage::OPCODE_REQUEST) {
        arp.target_ethernet_address = dst;
    }
    arp.target_ip_address = dst_ip;

    frame.header().src = _ethernet_address;
    frame.header().dst = dst;
    frame.header().type = EthernetHeader::TYPE_ARP;

    frame.payload() = arp.serialize();
    _frames_out.push(std::move(frame));
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    if (frame.header().dst != _ethernet_address && frame.header().dst != ETHERNET_BROADCAST)
        return nullopt;
    if (frame.header().type == EthernetHeader::TYPE_ARP) {
        ARPMessage arp;
        if (arp.parse(frame.payload()) != ParseResult::NoError) {
            return std::nullopt;
        }
        if (arp.target_ip_address != _ip_address.ipv4_numeric()) {
            return std::nullopt;
        }
        if (arp.opcode == ARPMessage::OPCODE_REQUEST) {
            send_arp(ARPMessage::OPCODE_REPLY, arp.sender_ethernet_address, arp.sender_ip_address);
        }

        _ip_eth_map[arp.sender_ip_address] = {arp.sender_ethernet_address, _ms_clock};

        while (!_frames_track.empty()) {
            if (send_datagram(_frames_track.front().dgram, _frames_track.front().next_hop)) {
                _frames_track.pop();
            } else
                break;
        }

    } else if (frame.header().type == EthernetHeader::TYPE_IPv4) {
        InternetDatagram dgram;
        if (dgram.parse(frame.payload()) != ParseResult::NoError)
            return nullopt;
        return dgram;
    }

    return std::nullopt;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) { _ms_clock += ms_since_last_tick; }
```

