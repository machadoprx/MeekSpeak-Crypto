#include "ecc_25519.h"

void
ecp_add(ec_t *curve, ecp_t *P, ecp_t *Q, big_t *p, ecp_t *R)
{
    ecp_null(R);

    big_t t1, t2, t3;

    big_mul_25519(&P->X, &Q->X, p, &t1);
    big_mul_25519(&P->Z, &Q->Z, p, &t2);
    big_sub_25519(&t1, &t2, p, &t3);
    big_mul_25519(&t3, &t3, p, &t2);
    big_sum_25519(&t2, &t2, p, &t3);
    big_sum_25519(&t3, &t3, p, &R->X);
    big_mul_25519(&P->X, &Q->Z, p, &t2);
    big_mul_25519(&P->Z, &Q->X, p, &t3);
    big_sub_25519(&t2, &t3, p, &t1);
    big_mul_25519(&t1, &t1, p, &t2);
    big_sum_25519(&t2, &t2, p, &t3);
    big_sum_25519(&t3, &t3, p, &t1);
    big_mul_25519(&t1, &curve->G.X, p, &R->Z);
}

void
ecp_dbl(ec_t *curve, ecp_t *P, big_t *p, ecp_t *R)
{
    ecp_null(R);

    big_t t1, t2, t3, x2, z2, xz;
    big_mul_25519(&P->X, &P->X, p, &x2);
    big_mul_25519(&P->Z, &P->Z, p, &z2);
    big_sub_25519(&x2, &z2, p, &t1);
    big_mul_25519(&t1, &t1, p, &R->X);
    big_mul_25519(&P->X, &P->Z, p, &xz);
    big_mul_25519(&curve->A, &xz, p, &t2);
    big_sum_25519(&t2, &x2, p, &t1);
    big_sum_25519(&t1, &z2, p, &t2);
    big_sum_25519(&xz, &xz, p, &t1);
    big_sum_25519(&t1, &t1, p, &t3);
    big_mul_25519(&t3, &t2, p, &R->Z);
}

static void
cst_swap(int swap, big_t* a, big_t *b)
{
    uint32_t dummy, mask = 0 - swap;

    for (int i = 0; i < 8; i++) {
        dummy = mask & (a->value[i] ^ b->value[i]);
        a->value[i] ^= dummy;
        b->value[i] ^= dummy;
    }
}

void
ecp_mul_cst(ec_t *curve, ecp_t *P, big_t *k, big_t *p, ecp_t *R)
{
    ecp_null(R);

    int bit_len;
    big_t x1, x2, z2, x3, z3, a24;
    uint32_t swap = 0;
    uint8_t kbits[512];
    big_to_bin(k, &bit_len, kbits);
    uint8_t *bit = kbits;
    big_t afn;
    ecp_get_afn(P, p, &afn);
    big_null(&x1);
    big_null(&x3);
    big_null(&a24);
    big_null(&x2);
    big_null(&z2);
    big_null(&z3);
    (*a24.value) = 121665u;
    (*x2.value) = 0x01u;
    (*z3.value) = 0x01u;

    big_cpy(&afn, &x1);
    big_cpy(&afn, &x3);

    for (int i = bit_len; i > 0; i--, bit++) {
        
        swap ^= (uint32_t)*bit;
        cst_swap(swap, &x2, &x3);
        cst_swap(swap, &z2, &z3);
        swap = (uint32_t)*bit;

        big_t A, AA, B, BB, C, D, E, DA, CB, t1, t2;
        big_sum_25519(&x2, &z2, p, &A);
        big_mul_25519(&A, &A, p, &AA);
        big_sub_25519(&x2, &z2, p, &B);
        big_mul_25519(&B, &B, p, &BB);
        big_sub_25519(&AA, &BB, p, &E);
        big_sum_25519(&x3, &z3, p, &C);
        big_sub_25519(&x3, &z3, p, &D);
        big_mul_25519(&D, &A, p, &DA);
        big_mul_25519(&C, &B, p, &CB);
        big_sum_25519(&DA, &CB, p, &t1);
        big_mul_25519(&t1, &t1, p, &x3);
        big_sub_25519(&DA, &CB, p, &t1);
        big_mul_25519(&t1, &t1, p, &t2);
        big_mul_25519(&t2, &x1, p, &z3);
        big_mul_25519(&AA, &BB, p, &x2);
        big_mul_25519(&a24, &E, p, &t1);
        big_sum_25519(&t1, &AA, p, &t2);
        big_mul_25519(&t2, &E, p, &z2);
    }

    cst_swap(swap, &x2, &x3);
    cst_swap(swap, &z2, &z3);
    big_cpy(&x2, &R->X);
    big_cpy(&z2, &R->Z);
}

void
ecp_mul(ec_t *curve, ecp_t *P, big_t *k, big_t *p, ecp_t *R)
{
    ecp_null(R);
    // no sanity check for k == 0 / do it in call
    int bit_len;
    ecp_t R0, R1, t1;
    uint8_t kbits[512];
    big_to_bin(k, &bit_len, kbits);
    uint8_t *bit = kbits + 1;

    ecp_cpy(P, &R1); // manual first op
    ecp_cpy(&R1, &R0);
    ecp_dbl(curve, &R1, p, &t1);
    ecp_cpy(&t1, &R1);
    bit_len--;

    for (int i = bit_len; i > 0; i--, bit++) {
        if (*bit != 0) {
            ecp_add(curve, &R0, &R1, p, &t1); 
            ecp_cpy(&t1, &R0);
            ecp_dbl(curve, &R1, p, &t1);
            ecp_cpy(&t1, &R1);
        }
        else {
            ecp_add(curve, &R0, &R1, p, &t1);
            ecp_cpy(&t1, &R1);
            ecp_dbl(curve, &R0, p, &t1);
            ecp_cpy(&t1, &R0);
        }
    }

    ecp_cpy(&R0, R);
}

void
ecp_get_afn(ecp_t *P, big_t *p, big_t *r)
{
    big_null(r);

    big_t t1, t2;
    big_mod_inv(&P->Z, p, &t1);
    big_mul(&P->X, &t1, &t2);
    big_mod_25519(&t2, p, r);
}
