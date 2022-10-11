#ifndef LOG_H
#define LOG_H

#include <syslog.h>
#include <cstdarg>  // 可变参数

// 设置日志级别
void set_loglevel(int log_level = LOG_DEBUG);
// 记录日志信息
void log(int log_level, const char* file_name, int line_num, const char* format, ...);

#endif