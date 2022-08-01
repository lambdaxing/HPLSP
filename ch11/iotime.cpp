#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>

#define TIMEOUT 5000

int main(int argc, char* argv[]) {
    int timeout = TIMEOUT;
    time_t start = time(NULL);
    time_t end = time(NULL);
    while(1) {
        printf("the timeout is now %d mil-seconds\n", timeout);
        start = time(NULL);
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, timeout);
        if((number < 0) && (errno != EINTR)) {
            printf("epoll failure\n");
            break;
        }
        /* 如果 epoll_wait 成功返回 0，则说明超时时间到，此时便可处理定时任务，并重置定时时间 */
        if(number == 0) {
            timeout = TIMEOUT;
            continue;
        }

        end = time(NULL);
        /* 如果 epoll_wai 的返回值大于 0，则本次 epoll_wait 调用持续的时间是 （end - start）* 1000 ms，我们需要将定时时间 timeout 减去这段时间，以获得下次 epoll_wait 调用的超时参数。 */
        timeout -= (end - start) * 1000;
        /* 重新计算之后的 timeout 值有可能等于 0，说明本次 epoll_wait 调用返回时，不仅有文件描述符就绪，而且其超时时间也刚好到达，此时我们也要处理定时任务，并重置定时时间 */
        if(timeout <= 0) {
            timeout = TIMEOUT;
        }

        // handle connection
    }

}