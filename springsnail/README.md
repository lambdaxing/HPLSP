# [负载均衡服务器](https://github.com/lambdaxing/HPLSP/tree/master/springsnail)

基于 Linux 的简易负载均衡服务器，从所有逻辑服务器中选取负荷最小的一台来处理新的客户连接。

## 项目概述：
+ 日志模块：log.h log.cpp
+ 连接对象：conn.h conn.cpp
+ 逻辑服务器处理对象：mgr.h mgr.cpp
+ 进程池： processpool.h
+ 配置文件：config.h config.cpp

## 配置文件: 
config.cpp

## 编译
```shell
    $ make
    $ ...
    $ ./springsnail
```