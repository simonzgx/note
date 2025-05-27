//
// Created by Administrator on 2020/8/19.
//

#ifndef NET_LOGGER_H
#define NET_LOGGER_H

#include <iostream>



#ifdef  DEBUG
#define LOG_SYSFATAL(format, ...)               \
    do { \
        fprintf(stderr, "[%s|%s@%s,%d] " format "\n",   \
        "SYSFATAL", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false); \

#define LOG_SYSERR(format, ...)               \
    do { \
        fprintf(stderr, "[%s|%s@%s,%d] " format "\n",   \
        "SYSERR", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false); \

#define LOG_ERROR(format, ...)               \
    do { \
        fprintf(stderr, "[%s|%s@%s,%d] " format "\n",   \
        "ERROR", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false); \

#define LOG_WARN(format, ...)               \
    do { \
        fprintf(stdout, "[%s|%s@%s,%d] " format "\n",   \
        "WARN", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false); \

#define LOG_INFO(format, ...)               \
    do { \
        fprintf(stdout, "[%s|%s@%s,%d] " format "\n",   \
        "INFO", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false); \

#define LOG_TRACE(format, ...)               \
    do { \
        fprintf(stdout, "[%s|%s@%s,%d] " format "\n",   \
        "TRACE", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false);                          \

#define LOG_DEBUG(format, ...)               \
    do { \
        fprintf(stdout, "[%s|%s@%s,%d] " format "\n",   \
        "DEBUG", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false);
#else
#define LOG_SYSFATAL(format, ...)               \
    do { \
        fprintf(stderr, "[%s|%s@%s,%d] " format "\n",   \
        "SYSFATAL", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false); \

#define LOG_SYSERR(format, ...)               \
    do { \
        fprintf(stderr, "[%s|%s@%s,%d] " format "\n",   \
        "SYSERR", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false); \

#define LOG_ERROR(format, ...)               \
    do { \
        fprintf(stderr, "[%s|%s@%s,%d] " format "\n",   \
        "ERROR", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false); \

#define LOG_WARN(format, ...)               \
    do { \
        fprintf(stdout, "[%s|%s@%s,%d] " format "\n",   \
        "WARN", __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (false); \

#define LOG_INFO(format, ...)
#define LOG_TRACE(format, ...)
#define LOG_DEBUG(format, ...)
#endif

#define CHECK_NOTNULL(val) \
  ::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

#endif //NET_LOGGER_H
