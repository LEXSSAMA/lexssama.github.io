---
title: CS144-lab6
index_img: /Picture/CS144.jpg
date: 2021-04-19 16:58:39
tags:
- CS144
- lab4
categories:
- 计算机网络
- CS144
banner_img:
---

# 实现route.cc
这个lab是实现路由器,路由转发的功能,需要维护一个路由转发表，通过目标IP地址与路由转发表中存储的IP地址进行最长前缀匹配来决定数据报要从路由器上面的哪一个网络接口`Network Interface`发出。<br>

# 代码
**route.hh**<br>
```C++
class Router {
.....
    //! The router's collection of network interfaces
    struct Table {
        const uint32_t route_prefix;
        const uint8_t prefix_length;
        const std::optional<Address> next_hop;
        const size_t interface_num;
    };

    std::vector<AsyncNetworkInterface> _interfaces{};
    std::vector<Table> _route_table{};
....
   }
```
**route.cc**<br>
```C++
#include "router.hh"
#include <iostream>
using namespace std;

void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
         << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";

    // Your code here.
    _route_table.push_back(Table{route_prefix, prefix_length, next_hop, interface_num});
}

void Router::route_one_datagram(InternetDatagram &dgram) {
    int size = _route_table.size();
    int fix_interface = -1;
    uint8_t longest = 0;
    for (int i = 0; i < size; ++i) {
        if (_route_table[i].prefix_length >= longest) {
            uint32_t mask =
                _route_table[i].prefix_length == 0 ? 0 : (0xFFFFFFFF << (32 - _route_table[i].prefix_length));
            uint32_t tmp = dgram.header().dst & mask;
            if (tmp == (_route_table[i].route_prefix & mask)) {
                longest = _route_table[i].prefix_length;
                fix_interface = _route_table[i].interface_num;
            }
        }
    }
    if (fix_interface == -1 || dgram.header().ttl <= 1)
        return;
    --dgram.header().ttl;
    if (_route_table[fix_interface].next_hop.has_value())
        interface(fix_interface).send_datagram(dgram, _route_table[fix_interface].next_hop.value());
    else
        interface(fix_interface).send_datagram(dgram, Address::from_ipv4_numeric(dgram.header().dst));
}

void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}
```

# 结语
CS144的项目到这里算是告一段落了，前前后后大概做了20多天，理论上理解TCP/IP和动手写TCP/IP确实完全不同,真正学习计算机就是要动手写代码!!<br>
