#ifndef ECC_25519_H
#define ECC_25519_H
#include "../bn/bn.h"

#define Gx_25519 9u

void ecp_mul_cst (big_t *, big_t *, big_t *, big_t *);
void get_rnd_bytes(uint8_t *);
void mask_bytes(uint8_t *);
void get_scalar25519(uint32_t *);
void decode_uc(uint32_t *);

#endif