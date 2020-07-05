---
title: Spring Cloud Hystrix
index_img: /Picture/spring-cloud-hystrix.png
date: 2020-07-05 19:01:17
tags:
- 微服务学习
categories:
- 微服务学习
banner_img:
---
# Hystrix (还需要继续丰富）
服务熔断:提供线程池，不同的服务走不同的线程池，实现不同服务调用的隔离，避免了服务雪崩的问题
## Hystrix的使用详情
### １．创建请求命令
HystrixCommand 它用来封装具体的依赖服务调用逻辑
- 同步执行
- 异步执行
### ２．定义服务降级
fallback是Hystrix命令执行失败时使用的后备方法，用来实现服务的降级处理逻辑，在HystrixCommand中可以通过重载getFallback()方法来实现降级逻辑<br>
这些情况可以不去实现降级逻辑<br>
执行写操作的命令
执行批处理或离线计算的命令
### ３．异常处理
- 异常传播<br>
HystrixCommad实现run()方法中抛出异常时，除了HystrixBadRequestException之外，其他异常均会被Hystrix认为命令执行失败并触发服务降级的处理逻辑<br>
- 异常获取<br>
当Hystrix命令因为异常（除了HystrixBadRequest的异常）进入服务的降级逻辑之后，往往需要对不同异常做针对性处理
### ４．命令名称，分组以及线程池划分
通常情况下，尽量通过HystrixThreadPoolKey的方式来指定线程池的划分，而不是通过组名的默认方式实现划分，因为多个不同命令可能从业务逻辑上看属于同一个组,但是往往从实现本身上需要跟其他命令进行隔离<br>
- commandKey
- groupKey
- threadPoolKey
### ５．请求缓存
- 开启请求缓存功能<br>
当不同的外部请求处理逻辑调用了同一个依赖服务时，Hystrix会根据getCacheKey方法返回值来是否是重复请求，如果他们的cacheKey相同，那么该依赖服务只会在第一个请求到达时被真正地调用一次，另一个请求则是直接从请求缓存中返回结果<br>
- *所以通过开启缓存可以让我们实现Hystrix命令具备下面几项好处<br>*
	- 减少重复的请求数，降低依赖服务的并发度
	- 在同一用户请求的上下文中，相同的依赖服务返回数据始终保持一致
	- 请求缓存在run()和construct()执行之前生效，所以可以减少不必要的线程开销
	- 清理失效缓存功能
	- 在使用请求缓存时，如果只是读操作，那么不需要考虑缓存内容是否正确的问题，但是如果请求命令中还有更新数据的写操作，那么缓存中的数据就需要
	- 我们在进行写操作时进行及时的处理，以防止读操作的请求命令获取到了失效的数据<br>
- *使用注解实现请求缓存*
	- １．设置请求缓存＠CacheResult
	- ２．定义缓存Key
	- ３．缓存清理
### ６．请求合并
在高并发的情况下，因通信次数的增加，总的通信时间消耗将会变得不那么理想，同时，因为依赖服务线程池资源有限，将出现排队等待与响应延迟的情况，为了
优化这两个问题，Hystrix提供了HystrixCollapser来实现请求的合并，以减少通信消耗和线程数的占用
- **HystrixCollapser<br>**
HystrixCollapser实现了在HystrixCommand之前放置一个合并处理器，将处于一个很短的时间窗（默认１０秒）内对同一个依赖的多个请求进行整合并以批量
的方式发起请求的功能（服务提供方也需要提供相应的批量实现的接口）
- **请求合并的额外开销**<br>
虽然通过请求合并可以减少请求的数量以缓解依赖服务线程池的资源，但是在使用的时候也需要注意它所带来的额外开销：用于请求合并的延迟时间窗会使得依赖服务的请求延迟提高：本来只需要５ms，请求合并后需要１５ms<br>

*根据一下两个实际情况考虑是否使用请求合并器*<br>
- 请求命令本身的延迟如果依赖服务的请求命令本身就是一个高延迟的命令，那么请求合并器所带来的延迟就可以忽略<br>
- 延迟时间窗内的并发量,如果时间窗内只有１～２个请求，那么这样的依赖服务不适合使用请求合并器．这样的情况不单不能提高系统性能，反而会成为系统瓶颈
### ７．Hystrix仪表盘
Spring Cloud完美整合了它的仪表盘组件Hystrix Dashboard , 它主要用来实时监控Hystrix的各项指标信息．通过Hystrix Dashboard反馈的实时信息，
可以帮助我们快速发现系统中存在的问题，从而及时地采取应对措施<br>
- 默认的集群监控
- 指定的集群监控（Turbine)
- 单体应用的监控
#### Turbine
Spring Cloud在分装Turbine的时候，还封装了基于消息代理的收集实现RabbitMQ<br>
- １．创建HystrixCommand 或　HystrixObservableCommand 对象
表示对依赖服务的操作请求，同时传递所需要的参数
	- HystrixCommand：用在依赖服务返回单个操作结果的时候
	- HystrixObservableCommand:用在依赖的服务返回多个操作结果的时候
- ２．命令执行<br>

	- *HystrixCommand实现了下面两个执行方式*<br>
		- execute():同步执行，从依赖的服务返回一个单一的结果对象，或是在发生错误的时候抛出异常．
		- queue():异步执行，直接返回一个Future对象，其中包括了服务执行结束时要返回的单一结果对象

	- *HystrixObservableCommand实现了另外两种执行方式*<br>
		- Observe():返回Observable对象，它代表了操作的多个结果，它是一个Hot Observable<br>
		- toObservable():同样会返回Observable对象，也代表了操作的多个结果，但它返回的是一个Cold Observable

- ３．结果是否被缓存
若当前命令的请求缓存功能是被启用的，并且该命令缓存命中，那么缓存的结果会立即以Observable对象的形式返回．
- ４．断路器是否打开
在命令结果没有缓存命中时，Hystrix在执行命令前需要检查断路器是否为打开状态
如果断路器是打开的，那么Hystrix不会执行命令，而是转接到fallback处理逻辑(对应下面第８步)
如果断路器是关闭的，那么Hystrix会跳到第5步，检查是否有可用资源来执行命令
- ５．线程池／请求队列／信号量是否占满
如果与命令有关的线程和请求队列，或者信号量(不使用线程池的时候)已经被占满，那么Hystrix也不会执行命令，而是转接到fallback处理逻辑（对应下面第８步）
- ６．HystrixObservableCommand.construct()或HystrixCommand.run()
	- HystrixCommand.run():返回一个单一的结果，或者抛出异常fallbac;
	- HystrixObservableCommand,construct():返回一个Observable对象来发射多个结果，或通过onError发送错误通知

*如果run()和construct()方法执行时间超过了命令设置的超时阈值，当前处理线程将会抛出一个TimeoutException(如果该命令不在其自身的线程中执行，则会通过单独的计时线程来抛出）．在这种情况下，Hystrix会转接到fallback处理逻辑，同时，如果当前命令没有被取消或中断，那么他最终会忽略run()或者construct()方法返回
如果命令没有抛出异常，那么Hystrix在记录一些日志并采集监控报告将该结果返回*
- ７．计算断路器的健康度
Hystrix会将＂成功＂，＂失败＂，＂拒绝＂，＂超时＂等信息报告给断路器，而断路器会维护一组计数器来统计这些数据
断路器会使用这些统计数据来决定是否要将断路器打开，来对某个依赖服务的请求进行＂熔断／短路＂，直到恢复期结束后，根据统计数据判断如果还是未达到健康指标，就再次＂熔断／短路＂
- ８．fallback处理
当命令执行失败时，Hystrix会进入fallback尝试回退处理，我们通常也称该操作为＂服务降级＂．而能够引起服务降级的情况有下面几种<br>
	- 第４步，当前命令处于＂熔断／短路＂状态，断路器是打开的时候
	- 第５步，当前命令的线程池，请求队列或者信号量被占满的时候
	- 第６步，HystrixObservableCommand.construct()或HystrixCommand.run()抛出异常的时候．
- ９．返回成功的响应
