#ifndef FDWRAPPER_H
#define FDWRAPPER_H

// 连接读写的返回信息
enum RET_CODE { OK = 0, NOTHING = 1, IOERR = -1, CLOSED = -2, BUFFER_FULL = -3, BUFFER_EMPTY = -4, TRY_AGAIN };
// 
enum OP_TYPE { READ = 0, WRITE, ERROR };

// 设置文件描述符 fd 非阻塞
int setnonblocking(int fd);
// 向 epollfd 内核事件表注册 fd 上的读事件
void add_read_fd(int epollfd, int fd);
// 向 epollfd 内核事件表注册 fd 上的写事件
void add_write_fd(int epollfd, int fd);
// 从 epollfd 中移除 fd
void removefd(int epollfd, int fd);
// 从 epollfd 中移除 fd，并 close fd
void closefd(int epollfd, int fd);
// 修改 fd 在 epollfd 上的事件集 为 ev
void modfd(int epollfd, int fd, int ev);

#endif