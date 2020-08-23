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

void
chacha_enc(uint32_t key[8], uint32_t nonce[3], uint8_t *plain, uint8_t *cipher, unsigned len)
{
    // only for msg with msg.len % 64 == 0, user must pad plain_text
    memset(cipher, 0, sizeof(uint8_t) * len);
    int range = (int)(len / 64);
    for (int j = 0; j < range; j++) {
        uint32_t state[16];
        uint8_t key_stream[64];
        make_state(state, key, j + 1, nonce);
        chacha_block(state);
        u32_to_u8(state, key_stream, 16);
        int index = j * 64;
        for (int i = 0; i < 64; i++) {
            cipher[index + i] = plain[index + i] ^ key_stream[i]; 
        }
    }
}

void
poly1305_mac(uint32_t key[], uint32_t nonce[], uint8_t *mac_data, unsigned mac_len, uint8_t *tag)
{
    memset(tag, 0, sizeof(uint8_t) * 17);
    unsigned i;
    uint32_t poly_key[16];
    big_t p, a, c, r, s, n, t1;

    big_null(&c);
    big_null(&p);
    big_null(&s);
    big_null(&a);

    make_state(poly_key, key, 0, nonce);
    chacha_block(poly_key);

    memcpy(c.value, poly_key, sizeof(uint32_t) * 4);
    memcpy(s.value, poly_key + 4, sizeof(uint32_t) * 4);
    memcpy(p.value, P1305, sizeof(uint32_t) * 5);

    big_t clamp;
    hex_to_big("0ffffffc0ffffffc0ffffffc0fffffff", &clamp);
    big_and(&c, &clamp, &r);

    for (i = 1; i <= (unsigned)ceil(mac_len / 16); i++) {
        big_null(&n);

        u8_to_u32(mac_data + (i - 1), n.value, 4);
        n.value[0] |= 0x01;

        big_sum(&a, &n, &t1);
        big_mul(&t1, &r, &a);
        big_mod_1305(&a, &p, &t1);
        big_cpy(&t1, &a);
    }

    big_sum(&a, &s, &t1);
    u32_to_u8(t1.value, tag, 4);
}