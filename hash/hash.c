#include "hash.h"

void 
chacha20_block(uint32_t in[16], uint32_t out[16])
{
	int i;
	uint32_t old[16];

    memcpy(old, in, sizeof(uint32_t) * 16);

	for (i = 0; i < 10; i++) {
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
chacha20_enc(uint32_t key[8], uint32_t nounce[3], uint32_t msg[], uint32_t msg_bytes, uint32_t out[][16])
{
    int i = 4, j = 0, m = 0;
    uint32_t inp_block[16], hash_block[16], cipher_blocks[(msg_bytes >> 2) + 1][16];

    inp_block[0] = 0x61707865;
    inp_block[1] = 0x3320646e;
    inp_block[2] = 0x79622d32;
    inp_block[3] = 0x6b206574;

    for (; i < 12; i++) {
        inp_block[i] = key[i - 4];
    }

    inp_block[12] = 0;
    inp_block[13] = nounce[0];
    inp_block[14] = nounce[1];
    inp_block[15] = nounce[2];
    
    for (i = msg_bytes >> 2; i >= 0; i--, j++) {
        chacha20_block(inp_block, hash_block);
        inp_block[12]++;
        for (int k = 0; k < 16; k++) {
            out[j][k] = msg[m++] ^ hash_block[k];
            if (m > (msg_bytes >> 2)) {
                return;
            }
        }
        j++;
    }
}

void
sha3_256()
{
    return;
}

void
aes_256_soft()
{
    return;
}

void
aes_256_hard()
{
    return;
}