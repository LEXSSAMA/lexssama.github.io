---
title: The Network Core
date: 2020-02-28 19:36:11
tags: -Computer Network A Top-Down Approach

---

# The Network Core

The figure highlights the Network Core with thick, shaded lines.

![](The-Network-Core/2020-02-28 19-54-24 的屏幕截图.png)

## Packet Switching

To send a message from a source end system to a destination end system, the source breaks long message into smaller chunks of data known as **Packets**. Between source and destination, each packet travel through **communication links and packet switches**(for which there are two predominant types, routers and link-layer switches) .

### Store-and-Forward Transmission

Store-and-forward transmission means that the packet switch must receive the entire packet before it can begin to transmit the first bit of the packet onto the outbound link. 

To explore store-and-forward transmission in more detail. Consider a simple network consisting of two end systems connected by a single router as shown as figure below.

A router will typically have many incident link, since its job is to switch an incoming packet onto an outgoing link;  In this example the router has the rather simple task of transferring a packet from one(input) link to the only other attached link. In this example the source has three packets, each consisting of L bits to send to the destination. Because the router employ store-and-forwarding, at this instant of time, the router cannot transmit the bits it has received; only after the router has received all of the packet's bits can it begin to transmit the packet onto the outbound link.

![s](The-Network-Core/2020-02-28 19-47-00 的屏幕截图.png)

To gain some insight into store-and-forward transmission, let's now calculate the amount of time that elapses from when the source begins to send the packet until the destination has received the entire packet.(Here we will ignore propagation delay - the time it take for the bits to travel across the wire at near the speed of light). The source begins to transmit at time 0; at time L/R seconds, the sources has transmitted the entire packet and the entire packet has been received and store at the router(since there is no propagation delay). At time L/R seconds, since the router  has just received the entire packet, it can begin to transmit the packet onto the outbound link towards the destination; at time 2L/R, the router have transmitted the entire packet, and the entire packet has been received by the desination. Thus the total delay is 2L/R. If the switch instead forwarded bits as soon as they arrive.(without first receving store and process the entire packet before forwarding) then the total delay would be L/R since bits are not help up at the router. But as we will discuss in Section1.4 routers need to receive, store,and process the entrie packet before forwarding.

let's now consider the general case of sending one packet from source to destination over a path consisting of N link each of rate R(thus, there are N-1 routers between source and destination). Applying the same logic as above, we see that the end-to-end delay is :
$$
d_ {end-to-end}= N*L/R
$$

### Queuing Delays and Packet Loss

Each packet switch has multiple link attached to it . For each attached link, the packet switch has an **output buffer**(also called an **output queue**),which stores packets that the router is about to send into that link. The output buffers play a key role in packet switching. If an arriving packet needs to be transmitted onto a link,but finds the link busy with the transmission of another packet, the arriving packets must wait in the output buffer. **Thus, in addition to the store-and-forward delays,packets suffer output buffer queuing delay.** These delay are variable and depend on the level of congestion in the network. since the amount of buffer space is finite. an arriving packet may find that the buffer is completely full with other packets waiting for transmission. In this case, **Packet Loss will occur** - either the arriving packet or one of the already-queued packets will be dropped.

### Forwarding Tables and Routing Protocols

Earlier, we said that a router takes a packet arriving on one of its attached communication links and forwards that packet onto another one of attached communication links. But how does the router determine which link it should forward the packet onto? Packet forwarding is actually done in different ways in different types of computer networks. Here, we briefly describe how it is done in the Internet.

​	In the Internet,every end system has an address called an **IP address.** when a source end system wants to send a packet to a destination end system, the source include the destination's IP address in the packet's header. As with postal addresses. each router has a **forwarding table** that maps destination addresses(or portions of the destination addresses) to that router's outbound links. When a packet arrives at a router, the router examines the address and searches its forwarding table, using this destination address, to find the appropriate outbound link. The router then directs the packet to this outbound link.

​	We just learned that a router uses a packet's destination address to index a forwarding table and determine the appropriate outbound link. But this statement begs yet another question: "How do forwarding tables get set ?", Are they configured by hand in each and every router, or does the Internet use a more automated procedure? The issue will be studied in depth in after. but we'll note now that the Internet has a number of special routing protocols that are used to automatically set the forwarding tables.

## Circuit Switching

In circuit-switching networds, the resource needed along a path(buffer link transmission rate) to provide for communication between the end systems are  reserved for the duration of the communication session between the end systems. In the packet-switched networks, these resources are not reserved; a session's messages use the resources on demand and as a consequence ,may have to wait for access to a  communication link.

​	Traditional telephone networks are examples of circuit-switched network. Consider what happens when one person want to send information(voice or facsimile) to another over a telephone network. Before the sender can send the information, the network must establish a connection between the sender and the receiver, for which the switches on the path between the sender and receiver maintain connection state for that connection.  In the jargon of telephony, this connection is called a **circuit**. when the network establishes the circuit , it also 