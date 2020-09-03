//
// Created by Administrator on 2020/8/19.
//

#ifndef NET_LOGGER_H
#define NET_LOGGER_H

#include <iostream>

#define LOG_SYSFATAL std::cout<<std::endl<<__FUNCTION__
#define LOG_SYSERR std::cout<<std::endl<<__FUNCTION__
#define LOG_TRACE std::cout<<std::endl<<__FUNCTION__
#define LOG_ERROR std::cout<<std::endl<<__FUNCTION__
#define LOG_DEBUG std::cout<<std::endl<<__FUNCTION__
#define LOG_FATAL std::cout<<std::endl<<__FUNCTION__
#define LOG_WARN std::cout<<std::endl<<__FUNCTION__
#define LOG_INFO std::cout<<std::endl<<__FUNCTION__

#define CHECK_NOTNULL(val) \
  ::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

#endif //NET_LOGGER_H
