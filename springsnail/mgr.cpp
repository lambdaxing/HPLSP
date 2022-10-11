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
    if(connect(sockfd, (struct sockaddr*)&address, sizeof(sockaddr)) != 0) {
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

    for(int i = 0; i < srv.m_conncnt; i++) {
        //sleep( 1 ); //emm怪不得创建连接这么慢。。。。
        int sockfd = conn2srv(address);
        if(sockfd < 0) {
            log(LOG_ERR, __FILE__, __LINE__, "build connection %d failed", i);
        }else {
            log( LOG_INFO, __FILE__, __LINE__, "build connection %d to server success", i );
            conn* tmp = NULL;
            try {
                tmp = new conn;
            }catch(...){
                close(sockfd);
                continue;
            }
            tmp->init_srv(sockfd, address);
            m_conns.insert(pair<int, conn*>(sockfd, tmp));
        }
    }
}

mgr::~mgr() {}

