#define ROT_L32(x, n) x = (x << n) | (x >> (32 - n))
#define QR(a, b, c, d)                 \
    a += b;  d ^= a;  ROT_L32(d, 16);  \
    c += d;  b ^= c;  ROT_L32(b, 12);  \
    a += b;  d ^= a;  ROT_L32(d,  8);  \
    c += d;  b ^= c;  ROT_L32(b,  7)

#ifndef STDINT_H
    #include <stdint.h>
#endif

#ifndef STRING_H
    #include <string.h>
#endif

void chacha_enc(uint32_t key[8], uint32_t nounce[3], uint32_t counter, uint32_t blocks, int rounds, uint32_t out[][16]);