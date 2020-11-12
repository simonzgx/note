//
// Created by Administrator on 2020/9/14.
//

#ifndef NET_PLATFORM_H
#define NET_PLATFORM_H

#if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)
#define __WINDOWS__
#endif

#if defined(__linux__) || defined(__CYGWIN__) || \
    defined(__GNU__) || defined(__GLIBC__)
#elif defined(__APPLE__)

#elif defined(__OpenBSD__) || defined(__FreeBSD__)

#elif defined(__sun)

#elif defined(__NetBSD__) || defined(__DragonFly__)

#elif defined(__WINDOWS__)

#else

#	error platform not supported

#endif

#endif //NET_PLATFORM_H
