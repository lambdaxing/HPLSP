#ifndef CONN_H
#define CONN_H

#include <arpa/inet.h>
#include "fdwrapper.h"

/* 连接类：表示客户端与服务器端的（一组）连接 */
class conn {
public:
    conn();
    ~conn();
    // 初始化客户端地址
    void init_clt(int sockfd, const sockaddr_in& client_addr);
    // 初始化服务器端地址
    void init_srv(int sockfd, const sockaddr_in& server_addr);
    void reset();
    // 从客户端读
    RET_CODE read_clt();
    // 向客户端写
    RET_CODE write_clt();
    // 从服务器端读
    RET_CODE read_srv();
    // 向服务器端写
    RET_CODE write_srv();
public:
    // 缓冲区大小
    static const int BUF_SIZE = 2048;

    // 客户端读缓冲区（服务器端写缓冲区）
    char* m_clt_buf;
    // 缓冲区读指针，指向已读入数据的下一字节
    int m_clt_read_idx;
    // 缓冲区写指针，指向已写入数据的下一字节
    int m_clt_write_idx;
    // 客户端地址
    sockaddr_in m_clt_address;
    // 客户端连接 socket 文件描述符
    int m_cltfd;

    // 服务器端读缓冲区（客户端写缓冲区）
    char* m_srv_buf;
    // 同上
    int m_srv_read_idx;
    // 同上
    int m_srv_write_idx;
    // 服务器端地址
    sockaddr_in m_srv_address;
    // 服务器端连接 socket 文件描述符
    int m_srvfd;

    // 服务器端连接是否关闭
    bool m_srv_closed;
};

#endif