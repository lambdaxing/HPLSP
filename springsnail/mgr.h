#ifndef MGR_H
#define MGR_H

#include <map>
#include <arpa/inet.h>
#include "fdwrapper.h"
#include "conn.h"

using std::map;

// 服务器端结构体
class host {
public:
    char m_hostname[1024];
    int m_port;
    int m_conncnt;
};

class mgr {
public:
    //在构造 mgr 的同时调用 conn2srv 和服务端建立连接
    mgr(int epollfd, const host& srv);
    ~mgr();
    // 和服务端建立连接同时返回 socket 描述符
    int conn2srv(const sockaddr_in& address);
    // 从连接好的连接中（m_conn中）拿出一个放入任务队列（m_used）中
    conn* pick_conn(int cltfd);
    void free_conn(conn* connection);
    int get_used_conn_cnt();
    void recycle_conns();
    RET_CODE process(int fd, OP_TYPE type);

private:
    static int m_epollfd;
    // 服务器端连接信息
    map<int, conn*> m_conns;
    map<int, conn*> m_used;
    map<int, conn*> m_freed;
    host m_logic_srv;
};

#endif