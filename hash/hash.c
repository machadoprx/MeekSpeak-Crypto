#include "hash.h"

void 
chacha_block(uint32_t in[16], int rounds, uint32_t out[16])
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
chacha_enc(uint32_t key[8], uint32_t nounce[3], uint32_t counter, uint32_t blocks, int rounds, uint32_t out[][16])
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
    inp_block[13] = nounce[0];
    inp_block[14] = nounce[1];
    inp_block[15] = nounce[2];
    
    for (int i = 0; i < blocks; i++) {
        chacha_block(inp_block, rounds, out[i]);
        inp_block[12]++;
    }
}