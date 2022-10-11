#include <stdio.h>
#include <time.h>
#include <string.h>
#include "log.h"

// 日志级别
static int level = LOG_INFO;
static const int LOG_BUFFER_SIZE = 2048;
static const char* loglevels[] =
{
    "emerge!", "alert!", "critical!", "error!", "warn!", "notice:", "info:", "debug:"
};

void set_loglevel(int log_level) {
    level = log_level;
}

void log(int log_level, const char* file_name, int line_num, const char* format, ...) {
    if(log_level > level) { // 忽略大于日志级别的日志信息
        return;
    }

    time_t tmp = time(NULL);    // 获取机器时间
    struct tm* cur_time = localtime(&tmp);
    if(!cur_time) {
        return;
    }

    char arg_buffer[LOG_BUFFER_SIZE];
    memset(arg_buffer, '\0', LOG_BUFFER_SIZE);
    strftime(arg_buffer, LOG_BUFFER_SIZE-1, "[ %x %X] ", cur_time);
    printf("%s", arg_buffer);
    printf("%s:%04d ", file_name, line_num);
    printf("%s ", loglevels[log_level - LOG_EMERG]);

    va_list arg_list;   // arg_list 指向可变参数 ... 部分
    // 用 va_start 宏初始化变量 arg_list，
    // 这个宏的第二个参数是第一个可变参数的前一个参数，是一个固定的参数
    va_start(arg_list, format);
    memset(arg_buffer, '\0', LOG_BUFFER_SIZE);
    vsnprintf(arg_buffer, LOG_BUFFER_SIZE-1, format, arg_list);
    printf("%s\n", arg_buffer);
    fflush(stdout);
    va_end(arg_list);
}