---
title: Spring Cloud Eureka 初入门
index_img: /Picture/Spring-Cloud-Eureka.png
date: 2020-06-17 17:38:42
tags:
- 微服务学习
categories:
- 微服务学习
banner_img:
---
# Eureka是什么？
Eureka就是由Netflix公司开发的一款开源的服务注册和发现的的产品 SpringCloud将其集成到SpringCloud的子项目Spring-Cloud-netflix中实现SpringCloud的服务注册和发现功能

# Eureka解决什么实际问题？
**Eureka解决的是服务的硬编码问题提供服务地址的问题,也就是微服务客户端不用在代码中硬写出微服务服务端，如果微服务客户端和服务端都有很多例如100个，那么每个微服物客户端都要写下100个服务端的IP地址以及端口,如果由于业务需求增加客户端或服务端，就又需要添加相应的代码，这样很难维护.**
# Eureka的划分
Eureka可以分为Eureka Service端和Eureka Client端<br>
Eureka Client可以建立在微服务的服务端(Application Service)和客户端(Application Client).<br>
# Eureka的架构

![eureka_architecture](eureka-architecture.png)<br>

简单的解释上图: 每一个Eureka Client(包括ApplicationService和ApplicationClient)需要向Eureka Service(服务注册中心)注册，当ApplicationClient需要访问ApplicationServices时，ApplicationClient就会向Eureka Service发出GET请求获取ApplicationService的信息，然后再进行远程调用Make Remote Call<br>

 **这样做的好处是什么呢？**<br>
*答:如果没有Eureka Service担任ApplicationService和ApplicationClient的第三方管理,那么ApplicationClient就需要在本地记住每一个ApplicationService的访问地址和端口，如果ApplicationService宕机或者开发者加入一个新的ApplicationService，这时就需要去修改每一个ApplicationClient的本地配置信息，如果ApplicationClient的数量很多例如100个，这样就会造成很大的工作量，另外Eureka还可以提供负载均衡的功能.*
# Eureka的功能
**Eureka Client:** 其实就是已经在注册中心注册了的微服务，Eureka Client内置负载均衡功能．<br>
**Eureka Service:** 提供服务注册功能，用来记录已注册微服务的相关信息.<br>
## Eureka的服务的基本流程
首先要写一个Eureka Service (服务注册中心) 提交上线例如上线到localhost:8761,接着需要写两个Eureka Client 分别是(Application Service 和 Application Client) , 分别通过Rest API的形式向Eureka Service注册,Eureka Service得到两个Eureka Client的相关信息，同时两个Eureka Client也获得了已在Eureka Service 中注册过得服务注册列表信息，这是ApplicationClient就知道了ApplicationService的IP地址，就可以通过HTTP远程调度来访问ApplicationService.<br>

有关RESI API 的内容可以看这篇文章:[Understanding And Using REST APIs](https://www.smashingmagazine.com/2018/01/understanding-using-rest-api/)

## Eureka Renew（服务续约)
Eureka Client默认每30秒发送一次心跳给Eureka Service 进行服务续约(告诉Eureka Service 我还活着) Eureka Service如果90秒没有收到Eureka Client的心跳，Eureka Service就会自动删除这个Eureka Client.

## Eureka Fetch Registries(获取服务列表信息)
Eureka Client本地有服务注册列表的的缓存信息而且默认每30秒Eureka Client会更新一次服务注册列表信息,Eureka Service中缓存了所有的注册信息，Eureka Client和Eureka Service可以通过XML或JSON的格式来通信，默认是JSON.
## Eureka Cancel(服务下线)
Eureka Cancel在程序关闭时可以向Eureka Service发送下线请求，Eureka Service会将该Eureka Client的相关信息删除，该功能不会自动完成，需要在Eureka Client 程序中调用相关代码．

## Eureka Eviction(服务剔除)
当Eureka Client连续默认在90秒内没有给Eureka Service的发送心跳Eureka Service会把该Eureka Client实例剔除．

## Eureka的自我保护
正常情况下如果默认90秒内Eureka Service没有收到心跳，就会删除相关Eureka Client实例，但是有时并非Eureka Client宕机，而是由于网络原因导致Eureka Service大面积丢失Eureka Client,这时如果Eureka Service收到的心跳小于某个阈值，Eureka Service就会开启自我保护机制：即Eureka Service只能读写而不能执行删除操作，当Eureka Service收到的心跳高于该阈值其就会自动退出自我保护机制．
Eureka Service的阈值默认是0.85,默认自动开启.

# Eureka的优点以及不足
*在了解Eureka的优点以及不知之前首先要先了解CAP原则．*

## CAP原则
CAP原则指的是在一个分布式系统中Consistency(一致性),Availability(可用性)
Partition tolerance(分区容错性).<br>

- **Consistency(一致性):** 分布式系统中的所有数据备份，在同一时刻是否有同样的值,例如当服务器A和服务器B中的数据应该保持一致，当服务器A中的数据改变时，服务器B应该迅速同步.
- **Avaliability(可用性):** 意思就是服务器只要收到用户的请求，就必须立刻给用户做出回应．
- **Partition tolerance (分区容错性):** 大多数分布式系统都分布在多个子网络中，例如一个服务器A坐落在北京一个服务器B在广州，服务器A,B之间有可能无法通信，所以分区容错在CAP中是无法避免的.

**CAP中P(分区容错)是无法避免的（即一定要保证分区容错性）同时可以看出C(一致性)和A(高可用性)是没有办法兼得，因为当服务A要与服务器B同步消息的某个时刻，用户向未更新消息的服务器B发送指令此时如果要保证一致性，服务器B就会忽略用户的请求，但是如果要保证高可用性,那么服务器B就必须立刻回应用户．**<br>

更详细可以看阮一峰大大的博客:
[CAP 定理的含义](http://www.ruanyifeng.com/blog/2018/07/cap.html)

## Eureka的优点
eureka 是保证AP,因此是保证可用性。Eureka Service几个节点宕机不会影响正常节点的工作，剩余的节点依然可以提供注册和查询服务。而eureka Client向某个eureka Service注册时如果发现连接失败，则会自动切换至其他Eureka Service。只要有一台eureka server 是正常的，就能保证服务可用(保证可用性)。不过查询到的信息可能不是最新的(不保证一致性)。

## Eureka的缺点
很明显Eureka的缺点是不保证一致性．

## Eureka的竞品：Zookeeper
Zookeeper是保证CP,即保证一致性,即任何时间对Zookeeper的任何服务器访问请求都能到一致的数据结果.这个就不详细讲了.



感谢:<br>
[Eureka 与 zookeeper 的区别、原理及各自优缺点](http://www.jeepxie.net/article/667918.html)<br>
[Spring Cloud Eureka 全解](https://zhuanlan.zhihu.com/p/34976125)<br>
[Spring Cloud-Eureka学习笔记-简介](https://zhuanlan.zhihu.com/p/120377144)
