#ifndef MGR_H
#define MGR_H

#include <map>
#include <arpa/inet.h>
#include "fdwrapper.h"
#include "conn.h"       // 逻辑连接类，其对象表示一条从客户到逻辑服务器的连接

using std::map;

// 逻辑服务器结构体
struct host {
    char m_hostname[1024];  // ip 地址
    int m_port;             // 端口号
    int m_conncnt;          // 支持连接数
};

// 逻辑服务器处理类
class mgr {
public:
    //在构造 mgr 的同时调用 conn2srv 和服务端建立连接
    mgr(int epollfd, const host& srv);
    ~mgr();
    // 和服务端建立连接同时返回 socket 描述符
    int conn2srv(const sockaddr_in& address);
    // 从连接好的连接中（m_conn中）拿出一个处理客户连接 cltfd，
    // 并将其放入任务队列（m_used）中
    conn* pick_conn(int cltfd);
    // 将连接加入回收池
    void free_conn(conn* connection);
    // 获取逻辑服务器当前的连接数
    int get_used_conn_cnt();
    // 回收已用连接池，m_freed -> m_conns
    void recycle_conns();
    // 处理文件描述符 fd（客户端/服务器端 socket）的 type 事件
    RET_CODE process(int fd, OP_TYPE type);

private:
    // 当前逻辑服务器处理（进程）的 I/O 复用 epoll 文件描述符
    // 用来进行 I/O 事件的注册
    static int m_epollfd;
    // 可用连接池
    map<int, conn*> m_conns;
    // 正在使用的连接（任务队列）
    map<int, conn*> m_used;
    // 已用连接池（等待被回收利用）
    map<int, conn*> m_freed;
    // 逻辑服务器处理类的逻辑服务器
    host m_logic_srv;
};

#endif