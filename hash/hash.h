#ifndef HASH_H
#define HASH_H

#include "../bn/bn.h"
#include "../utils.h"
#include <math.h>
#include <string.h>
#include <stdint.h>

#define ROT_L32(x, n) x = (x << n) | (x >> (32 - n))
#define QR(a, b, c, d)                 \
    a += b;  d ^= a;  ROT_L32(d, 16);  \
    c += d;  b ^= c;  ROT_L32(b, 12);  \
    a += b;  d ^= a;  ROT_L32(d,  8);  \
    c += d;  b ^= c;  ROT_L32(b,  7)

#define ROUNDS 10

void chacha_enc(uint32_t key[], uint32_t nonce[], uint8_t *plain, uint8_t *cipher, unsigned len);
void chacha_block(uint32_t state[]);
void poly1305_mac(uint32_t key[], uint32_t nonce[], uint8_t *msg, unsigned msg_len, uint8_t *tag);

#endif