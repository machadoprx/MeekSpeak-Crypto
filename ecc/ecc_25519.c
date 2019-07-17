#include "ecc_25519.h"

ecp_t*
ecp_new()
{
    ecp_t *P = calloc(1, sizeof(ecp_t));
    ecp_null(P);
    return P;
}

ec_t*
ec_init_c25519()
{
    ec_t *curve = malloc(sizeof(ec_t));
    big_null(&curve->A);
    curve->G = ecp_new();
    *(curve->G->X.value) = GX_25519;
    *(curve->G->Z.value) = GZ_25519;
    *(curve->A.value) = GA_25519;
    return curve;
}

void
ecp_null(ecp_t *P)
{
    memset(P->X.value, 0, BIG_WORDS_SIZE);
    memset(P->Z.value, 0, BIG_WORDS_SIZE);
    P->X.sign = false;
    P->Z.sign = false;
}

void
ecp_cpy(ecp_t *P, ecp_t *R)
{
    memcpy(R->X.value, P->X.value, BIG_WORDS_SIZE);
    memcpy(R->Z.value, P->Z.value, BIG_WORDS_SIZE);
    R->X.sign = P->X.sign;
    R->Z.sign = P->Z.sign;
}

void
ecp_add(ec_t *curve, ecp_t *P, ecp_t *Q, big_t *p, ecp_t *R)
{
    ecp_null(R);

    big_t t1, t2, t3;

    big_mul(&P->X, &Q->X, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_mul(&P->Z, &Q->Z, &t1);
    big_fastmod_25519(&t1, p, &t3);

    big_sub(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_mul(&t2, &t2, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_sum(&t2, &t2, &t3);
    big_sum(&t3, &t3, &t1);
    big_fastmod_25519(&t1, p, &R->X);

    big_mul(&P->X, &Q->Z, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_mul(&P->Z, &Q->X, &t1);
    big_fastmod_25519(&t1, p, &t3);

    big_sub(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_mul(&t2, &t2, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_sum(&t2, &t2, &t3);
    big_sum(&t3, &t3, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_mul(&t2, &curve->G->X, &t1);
    big_fastmod_25519(&t1, p, &R->Z);

}

void
ecp_double(ec_t *curve, ecp_t *P, big_t *p, ecp_t *R)
{
    ecp_null(R);

    big_t t1, t2, t3, x2, z2, xz;
    
    big_mul(&P->X, &P->X, &t1);
    big_fastmod_25519(&t1, p, &t2);
    big_cpy(&t2, &x2);

    big_mul(&P->Z, &P->Z, &t1);
    big_fastmod_25519(&t1, p, &t3);
    big_cpy(&t3, &z2);

    big_sub(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_mul(&t2, &t2, &t1);
    big_fastmod_25519(&t1, p, &R->X);

    big_mul(&P->X, &P->Z, &t1);
    big_fastmod_25519(&t1, p, &xz);

    big_mul(&curve->A, &xz, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_sum(&t2, &x2, &t1);
    big_fastmod_25519(&t1, p, &t2);

    big_sum(&t2, &z2, &t1);
    big_fastmod_25519(&t1, p, &t2); //guarda

    big_sum(&xz, &xz, &t3);
    big_sum(&t3, &t3, &t1);
    big_fastmod_25519(&t1, p, &t3);

    big_mul(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, &R->Z);
}

void
ecp_mul(ec_t *curve, ecp_t *P, big_t *k, big_t *p, ecp_t *R)
{
    ecp_null(R);
    // no sanity check for k == 0 / do it in call
    int bit_len;
    ecp_t R0, R1, t1;
    
    dig_t *kbits = big_to_bin(k, &bit_len);
    dig_t *bit = kbits + 1;

    ecp_cpy(P, &R1); // manual first op
    ecp_cpy(&R1, &R0);
    ecp_double(curve, &R1, p, &t1);
    ecp_cpy(&t1, &R1);
    bit_len--;

    for (int i = bit_len; i > 0; i--, bit++) {
        if (*bit != 0) {
            ecp_add(curve, &R0, &R1, p, &t1); 
            ecp_cpy(&t1, &R0);
            ecp_double(curve, &R1, p, &t1);
            ecp_cpy(&t1, &R1);
        }
        else {
            ecp_add(curve, &R0, &R1, p, &t1);
            ecp_cpy(&t1, &R1);
            ecp_double(curve, &R0, p, &t1);
            ecp_cpy(&t1, &R0);
        }
    }

    ecp_cpy(&R0, R);
    free(kbits);
}

void
ecp_get_affine(ecp_t *P, big_t *p, big_t *r)
{
    big_null(r);

    big_t t1, t2;
    big_mod_inv(&P->Z, p, &t1);
    big_mul(&P->X, &t1, &t2);
    big_fastmod_25519(&t2, p, r);
}
