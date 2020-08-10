#ifndef UTIL_SHA256_H
#define UTIL_SHA256_H

#include <cstddef>
#include <cstdint>

namespace util {

#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest
    
    typedef uint8_t BYTE;   // 8-bit byte
    typedef uint32_t WORD;  // 32-bit word

    typedef struct {
        BYTE data[64];
        WORD datalen;
        unsigned long long bitlen;
        WORD state[8];
    } SHA256_CTX;

    void sha256_init(SHA256_CTX *ctx);

    void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);

    void sha256_final(SHA256_CTX *ctx, BYTE hash[]);
}


#endif   // UTIL_SHA256_H
