#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <exception>

#include "log.h"
#include "mgr.h"

using std::pair;

int mgr::m_epollfd = -1;

int mgr::conn2srv(const sockaddr_in& address) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        return -1;
    }

    // 同逻辑服务器相连接
    if(connect(sockfd, (struct sockaddr*)&address, sizeof(address)) != 0) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

mgr::mgr(int epollfd, const host& srv) : m_logic_srv(srv) {
    m_epollfd = epollfd;
    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, srv.m_hostname, &address.sin_addr);
    address.sin_port = htons(srv.m_port);
    log(LOG_INFO, __FILE__, __LINE__, "logical srv host info: (%s, %d)", srv.m_hostname, srv.m_port);
    
    // 建立连接池，m_conncnt 个连接
    for(int i = 0; i < srv.m_conncnt; i++) {
        //sleep( 1 ); //emm怪不得创建连接这么慢。。。。
        int sockfd = conn2srv(address);
        if(sockfd < 0) {
            log(LOG_ERR, __FILE__, __LINE__, "build connection %d failed", i);
        }else {
            log( LOG_INFO, __FILE__, __LINE__, "build connection %d to server success", i );
            conn* tmp = NULL;
            try {
                // 创建连接对象，此时为半连接对象（未使用）
                tmp = new conn;
            }catch(...){
                close(sockfd);
                continue;
            }
            tmp->init_srv(sockfd, address);
            // 插入连接池
            m_conns.insert(pair<int, conn*>(sockfd, tmp));
        }
    }
}

mgr::~mgr() {
    // 清空 m_conn
    for(auto iter = m_conns.begin(); iter != m_conns.end(); iter++) {
        conn* tmp = iter->second;
        delete tmp;
    }
    m_conns.clear();
    // 清空 m_used
    for(auto iter = m_used.begin(); iter != m_used.end(); iter++) {
        conn* tmp = iter->second;
        if(tmp == NULL)
            continue;
        if(iter->first == tmp->m_cltfd) {
            m_used[tmp->m_srvfd] = NULL;
        }else {
            m_used[tmp->m_cltfd] = NULL;
        }
        delete tmp;
        m_used[iter->first] = NULL;
    }
    m_used.clear();
    // 清空 m_freed
    for(auto iter = m_freed.begin(); iter != m_freed.end(); iter++) {
        conn* tmp = iter->second;
        delete tmp;
    }
    m_freed.clear();
}

int mgr::get_used_conn_cnt() {
    return m_used.size();
}

conn* mgr::pick_conn(int cltfd) {
    // 连接池空
    if(m_conns.empty()) {
        log(LOG_ERR, __FILE__, __LINE__, "%s", "not enough srv connections to server");
        return NULL;
    }

    // 从连接池中取出一个未使用的连接
    map<int, conn*>::iterator iter = m_conns.begin();
    int srvfd = iter->first;
    conn* tmp = iter->second;
    if(!tmp) {
        log(LOG_ERR, __FILE__, __LINE__, "%s", "empty server connection object");
        return NULL;
    }
    m_conns.erase(iter);
    // 连接转移
    m_used.insert(pair<int, conn*>(cltfd, tmp));
    m_used.insert(pair<int, conn*>(srvfd, tmp));
    add_read_fd(m_epollfd, cltfd);
    add_read_fd(m_epollfd, srvfd);
    log(LOG_INFO, __FILE__, __LINE__, "bind client sock %d with server sock %d", cltfd, srvfd);
    return tmp;
}

// 关闭连接，进入回收池
void mgr::free_conn(conn* connection) {
    int cltfd = connection->m_cltfd;
    int srvfd = connection->m_srvfd;
    closefd(m_epollfd, cltfd);
    closefd(m_epollfd, srvfd);
    m_used.erase(cltfd);
    m_used.erase(srvfd);
    connection->reset();
    m_freed.insert(pair<int, conn*>(srvfd, connection));
}

void mgr::recycle_conns() {
    if(m_freed.empty()) {
        return;
    }
    for(auto iter = m_freed.begin(); iter != m_freed.end(); iter++) {
        //sleep(1);
        int srvfd = iter->first;
        conn* tmp = iter->second;
        srvfd = conn2srv(tmp->m_srv_address);
        if(srvfd < 0) {
            log(LOG_ERR, __FILE__, __LINE__, "%s", "fix connection failed");
        }else {
            log(LOG_INFO, __FILE__, __LINE__, "%s", "fix connection success");
            tmp->init_srv(srvfd, tmp->m_srv_address);
            m_conns.insert(pair<int, conn*>(srvfd, tmp));
        }
    }
    m_freed.clear();
}

RET_CODE mgr::process(int fd, OP_TYPE type) {
    conn* connection = m_used[fd];
    if(!connection) {
        return NOTHING;
    }
    if(connection->m_cltfd == fd) {
        int srvfd = connection->m_srvfd;
        switch (type)
        {
        case READ: {
            RET_CODE res = connection->read_clt();
            switch (res)
            {
            case OK: {
                log(LOG_DEBUG, __FILE__, __LINE__, "content read from client: %s", connection->m_clt_buf);
            }
            case BUFFER_FULL: {
                modfd(m_epollfd, srvfd, EPOLLOUT);
                break;
            }
            case IOERR:
            case CLOSED: {
                free_conn(connection);
                return CLOSED;
            }
            default:
                break;
            }
            if(connection->m_srv_closed) {
                free_conn(connection);
                return CLOSED;
            }
            break;
        }
        case WRITE: {
            RET_CODE res = connection->write_clt();
            switch (res)
            {
            case TRY_AGAIN: {
                modfd(m_epollfd, fd, EPOLLOUT);
                break;
            }
            case BUFFER_EMPTY: {
                modfd(m_epollfd, srvfd, EPOLLIN);
                modfd(m_epollfd, fd, EPOLLIN);
                break;
            }
            case IOERR:
            case CLOSED: {
                free_conn(connection);
                return CLOSED;
            }
            default:
                break;
            }
            if(connection->m_srv_closed) {
                free_conn(connection);
                return CLOSED;
            }
            break;
        }
        default: {
            log(LOG_ERR, __FILE__, __LINE__, "%s", "other operation not support yet");
            break;
        }
        }
    }else if(connection->m_srvfd == fd) {
        int cltfd = connection->m_cltfd;
        switch( type )
        {
            case READ:
            {
                RET_CODE res = connection->read_srv();
                switch( res )
                {
                    case OK:
                    {
                        log( LOG_DEBUG, __FILE__, __LINE__, "content read from server: %s", connection->m_srv_buf );  //此处的break不能加，在读完消息之后
                                                                                                                      //应该继续去触发BUFFER_FULL从而通知可写
                    }
                    case BUFFER_FULL:
                    {
                        modfd( m_epollfd, cltfd, EPOLLOUT );
                        break;
                    }
                    case IOERR:
                    case CLOSED:
                    {
                        modfd( m_epollfd, cltfd, EPOLLOUT );
                        connection->m_srv_closed = true;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case WRITE:
            {
                RET_CODE res = connection->write_srv();
                switch( res )
                {
                    case TRY_AGAIN:
                    {
                        modfd( m_epollfd, fd, EPOLLOUT );
                        break;
                    }
                    case BUFFER_EMPTY:
                    {
                        modfd( m_epollfd, cltfd, EPOLLIN );
                        modfd( m_epollfd, fd, EPOLLIN );
                        break;
                    }
                    case IOERR:
                    case CLOSED:
                    {
                        modfd( m_epollfd, cltfd, EPOLLOUT );
                        connection->m_srv_closed = true;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            default:
            {
                log( LOG_ERR, __FILE__, __LINE__, "%s", "other operation not support yet" );
                break;
            }
        }
    }else {
        return NOTHING;
    }
    return OK;
}