#ifndef HASH_H
#define HASH_H

#include "../bn/bn.h"

#define ROT_L32(x, n) x = (x << n) | (x >> (32 - n))
#define QR(a, b, c, d)                 \
    a += b;  d ^= a;  ROT_L32(d, 16);  \
    c += d;  b ^= c;  ROT_L32(b, 12);  \
    a += b;  d ^= a;  ROT_L32(d,  8);  \
    c += d;  b ^= c;  ROT_L32(b,  7)

#ifndef STDINT_H
#define STDINT_H
    #include <stdint.h>
#endif

#ifndef STRING_H
#define STRING_H
    #include <string.h>
#endif

#define ROUNDS 10

void chacha_enc(uint32_t key[], uint32_t nonce[], uint32_t counter, uint8_t *plain, uint8_t *cipher, int len);
//void poly1305_mac   (uint32_t [], uint32_t [], uint32_t [], int, uint32_t []);

#endif