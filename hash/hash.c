#include "hash.h"

void 
chacha_block(uint32_t state[])
{
	uint32_t old[16];

    memcpy(old, state, sizeof(uint32_t) * 16);

	for (int i = 0; i < ROUNDS; i++) {
		QR(old[0], old[4], old[8 ], old[12]); 
		QR(old[1], old[5], old[9 ], old[13]); 
		QR(old[2], old[6], old[10], old[14]); 
		QR(old[3], old[7], old[11], old[15]); 
		QR(old[0], old[5], old[10], old[15]);
		QR(old[1], old[6], old[11], old[12]);
		QR(old[2], old[7], old[8 ], old[13]);
		QR(old[3], old[4], old[9 ], old[14]);
	}

	for (int i = 0; i < 16; ++i) {
        state[i] += old[i];
    }
}

static inline void 
make_state(uint32_t state[], uint32_t key[], uint32_t counter, uint32_t nonce[])
{
    state[0] = 0xfa835867;
    state[1] = 0x2086ca69;
    state[2] = 0x1467c0fb;
    state[3] = 0x638e2b99;
    state[4] = key[0];
    state[5] = key[1];
    state[6] = key[2];
    state[7] = key[3];
    state[8] = key[4];
    state[9] = key[5];
    state[10] = key[6];
    state[11] = key[7];
    state[12] = counter;
    state[13] = nonce[0];
    state[14] = nonce[1];
    state[15] = nonce[2];
}

static inline void
serialize(uint32_t state[16], uint8_t stream[64])
{
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 4; j++) {
            int stream_index = (15 - i) * 4 + (3 - j);
            stream[stream_index] = (uint8_t)((state[i] >> (j * 8)) & 0xFF);
        }
    }
}

void
chacha_enc(uint32_t key[8], uint32_t nonce[3], uint8_t *plain, uint8_t *cipher, int len)
{
    int range = (int)(len / 64) - 1;
    for (int j = 0; j < range; j++) {
        uint32_t state[16];
        uint8_t key_stream[64];
        make_state(state, key, j, nonce);
        chacha_block(state);
        serialize(state, key_stream);

        int index = j * 64;
        for (int i = 0; i < 64; i++) {
            cipher[index + i] = plain[index + i] ^ key_stream[i]; 
        }
    }
}
/*
static void
poly1305_keygen(uint32_t key[], uint32_t nonce[], uint32_t r[], uint32_t s[])
{
    int i;
    big_t rtmp, clamp, res;
    uint32_t out[1][16];

    big_null(&rtmp);
    big_null(&clamp);
    big_null(&res);
    chacha_enc(key, nonce, 0, 1, 20, out);

    for (i = 0; i < 4; i++) {
        s[i] = (*out)[i + 8];
        rtmp.value[i] = (dig_t)(*out)[i + 12];
    }

    memcpy(clamp.value, C1305, sizeof(dig_t) * 4);
    big_and(&rtmp, &clamp, &res);

    for (i = 0; i < 4; i++) {
        r[i] = (uint32_t)res.value[i];
    }
}

void
poly1305_mac(uint32_t key[], uint32_t nonce[], uint32_t msg[], int msg_len, uint32_t auth[])
{
    int i;
    uint32_t raux[4], saux[4], naux[4];
    big_t p, a, r, s, n, t1;

    big_null(&p);
    big_null(&r);
    big_null(&s);
    big_null(&a);

    poly1305_keygen(key, nonce, raux, saux);
    for (i = 0; i < 4; i++) {
        r.value[i] = raux[i];
        s.value[i] = saux[i];
    }
    memcpy(p.value, P1305, sizeof(dig_t) * 5);

    for (i = 0; i < (msg_len >> 4); i++) {
        big_null(&n);
        memcpy(naux, msg + (i * 4), sizeof(uint32_t) * 4);
        msg[0] |= 0x01;
        for (int j = 0; j < 4; j++) {
            n.value[j] = msg[i];
        }
        big_sum(&a, &n, &t1);
        big_mul(&t1, &r, &a);
        big_mod_1305(&a, &p, &t1);
        big_cpy(&t1, &a);
    }
    big_sum(&a, &s, &t1);
    for (i = 0; i < 5; i++) {
        auth[i] = (uint32_t)t1.value[i];
    }
}*/