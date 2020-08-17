#ifndef ECC_25519_H
#define ECC_25519_H
#include "../bn/bn.h"

#define Gx_25519 9u

void ecp_mul_cst (big_t *, big_t *, big_t *, big_t *);

#endif