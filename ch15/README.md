# HPLSP
《Linux 高性能服务器编程》源码

## [Web 服务器](https://github.com/lambdaxing/HPLSP/tree/master/ch15)
* I/O 模型：I/O 复用 epoll
* 事件处理模式：用同步 I/O 模型模拟的 Proactor 模式 
    - 主线程完成数据的读写
* 并发模式：半同步/半反应堆模式 
    - 异步线程（主线程）用于处理 I/O 事件（I/O 处理单元）
    - 同步线程（工作线程）用于处理客户逻辑（逻辑单元）
* 逻辑处理方式：有限状态机
* 源码：
    - 主函数：[main.cpp](https://github.com/lambdaxing/HPLSP/tree/master/ch15/main.cpp)
    - 逻辑任务类：[http_conn.h](https://github.com/lambdaxing/HPLSP/tree/master/ch15/http_conn.h) [http_conn.cpp](https://github.com/lambdaxing/HPLSP/tree/master/ch15/http_conn.cpp)
    - 线程池类：[threadpool.h](https://github.com/lambdaxing/HPLSP/tree/master/ch15/threadpool.h)

## [CGI 服务器](https://github.com/lambdaxing/HPLSP/tree/master/ch15)
* I/O 模型：I/O 复用 epoll
* 事件处理模式：Reactor
* 并发模式：高效的半同步/半异步模式 
    - 主进程管理监听 socket、派发连接 socket
    - 工作进程管理连接 socket 及其上的 I/O 操作
* 源码：
    - 主函数：[cgi.cpp](https://github.com/lambdaxing/HPLSP/tree/master/ch15/cgi.cpp)
    - 进程池类：[processpool.h](https://github.com/lambdaxing/HPLSP/tree/master/ch15/processpool.h)

## 编译
```shell
$ g++ main.cpp http_conn.cpp -o main.out -lpthread -lrt -levent
```