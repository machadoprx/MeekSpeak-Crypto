#include "hash.h"

void 
chacha_block(uint32_t in[], int rounds, uint32_t out[])
{
	int i, end = rounds >> 1;
	uint32_t old[16];

    memcpy(old, in, sizeof(uint32_t) * 16);

	for (i = 0; i < end; i++) {
		QR(in[0], in[4], in[8 ], in[12]); 
		QR(in[1], in[5], in[9 ], in[13]); 
		QR(in[2], in[6], in[10], in[14]); 
		QR(in[3], in[7], in[11], in[15]); 
		QR(in[0], in[5], in[10], in[15]);
		QR(in[1], in[6], in[11], in[12]);
		QR(in[2], in[7], in[8 ], in[13]);
		QR(in[3], in[4], in[9 ], in[14]);
	}

	for (i = 0; i < 16; ++i) {
        out[i] = in[i] + old[i];
    }
}

void
chacha_enc(uint32_t key[], uint32_t nonce[], uint32_t counter, uint32_t blocks, int rounds, uint32_t out[][16])
{
    uint32_t inp_block[16];

    inp_block[0] = 0xfa835867;
    inp_block[1] = 0x2086ca69;
    inp_block[2] = 0x1467c0fb;
    inp_block[3] = 0x638e2b99;
    inp_block[4] = key[0];
    inp_block[5] = key[1];
    inp_block[6] = key[2];
    inp_block[7] = key[3];
    inp_block[8] = key[4];
    inp_block[9] = key[5];
    inp_block[10] = key[6];
    inp_block[11] = key[7];
    inp_block[12] = counter;
    inp_block[13] = nonce[0];
    inp_block[14] = nonce[1];
    inp_block[15] = nonce[2];
    
    for (int i = 0; i < blocks; i++) {
        chacha_block(inp_block, rounds, out[i]);
        inp_block[12]++;
    }
}

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
}