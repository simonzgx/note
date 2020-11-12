//
// Created by Administrator on 2020/8/19.
//

#ifndef NET_END_H
#define NET_END_H

#include <cstdint>

#include "portable_endian.h"


namespace net {
    namespace sockets {

// the inline assembler code makes type blur,
// so we disable warnings for a while.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"

        inline uint64_t hostToNetwork64(uint64_t host64, bool isBigEndian = true) {
            if (isBigEndian) {
                return htobe64(host64);
            }
            return htole64(host64);
        }

        inline uint32_t hostToNetwork32(uint32_t host32, bool isBigEndian = true) {
            if (isBigEndian) {
                return htobe32(host32);
            }
            return htole32(host32);
        }

        inline uint16_t hostToNetwork16(uint16_t host16, bool isBigEndian = true) {
            if (isBigEndian) {
                return htobe16(host16);
            }
            return htole16(host16);
        }

        inline uint64_t networkToHost64(uint64_t net64, bool isBigEndian = true) {
            if (isBigEndian) {
                return be64toh(net64);
            }
            return le64toh(net64);
        }

        inline uint32_t networkToHost32(uint32_t net32, bool isBigEndian = true) {
            if (isBigEndian) {
                return be32toh(net32);
            }
            return le32toh(net32);
        }

        inline uint16_t networkToHost16(uint16_t net16, bool isBigEndian = true) {
            if (isBigEndian) {
                return be16toh(net16);
            }
            return le16toh(net16);
        }

#pragma GCC diagnostic pop
    }
}


#endif //NET_END_H
