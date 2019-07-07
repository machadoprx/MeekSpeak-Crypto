#include "ecc_25519.h"

ecp_t*
ecp_new()
{
    ecp_t *P = malloc(sizeof(ecp_t));
    ecp_null(P);
    return P;
}

ec_t*
ec_init_c25519()
{
    ec_t *curve = malloc(sizeof(ec_t));
    big_null(&curve->A);
    curve->G = ecp_new();
    curve->G->X.value[0] = GX_25519;
    curve->G->Z.value[0] = GZ_25519;
    curve->A.value[0] = GA_25519;
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
ecp_add(ec_t *curve, ecp_t *P, ecp_t *Q, big_t *p, big_t *pn, ecp_t *R)
{
    ecp_null(R);

    if (*(P->X.value) == ECP_INF) {
        ecp_cpy(Q, R);
        return;
    }

    if (*(Q->X.value) == ECP_INF) {
        ecp_cpy(P, R);
        return;
    }

    big_t t1, t2, t3, four;
    big_null(&four);
    hex_to_big("4", &four);

    big_mul(&P->X, &Q->X, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&P->Z, &Q->Z, &t1);
    big_fastmod_25519(&t1, p, pn, &t3);

    big_sub(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&t2, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&t2, &four, &t1);
    big_fastmod_25519(&t1, p, pn, &R->X);

    big_mul(&P->X, &Q->Z, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&P->Z, &Q->X, &t1);
    big_fastmod_25519(&t1, p, pn, &t3);

    big_sub(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&t2, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&t2, &four, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&t2, &curve->G->X, &t1);
    big_fastmod_25519(&t1, p, pn, &R->Z);

}

void
ecp_double(ec_t *curve, ecp_t *P, big_t *p, big_t *pn, ecp_t *R)
{
    ecp_null(R);

    if (*(P->X.value) == ECP_INF) {
        *(R->X.value) = ECP_INF;
        *(R->Z.value) = ECP_INF;
        return;
    }

    big_t t1, t2, t3, x2, z2, xz, four;
    big_null(&four);
    hex_to_big("4", &four);
    
    big_mul(&P->X, &P->X, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_cpy(&t2, &x2);

    big_mul(&P->Z, &P->Z, &t1);
    big_fastmod_25519(&t1, p, pn, &t3);
    big_cpy(&t3, &z2);

    big_sub(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&t2, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &R->X);

    big_mul(&P->X, &P->Z, &t1);
    big_fastmod_25519(&t1, p, pn, &xz);

    big_mul(&curve->A, &xz, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_sum(&t2, &x2, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_sum(&t2, &z2, &t1);
    big_fastmod_25519(&t1, p, pn, &t2); //guarda

    big_mul(&xz, &four, &t1);
    big_fastmod_25519(&t1, p, pn, &t3);

    big_mul(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, pn, &R->Z);
}

void
ecp_mul(ec_t *curve, ecp_t *P, big_t *k, big_t *p, big_t *pn, ecp_t *R)
{
    int lenght = 0;
    ecp_t R0, R1, t1;
    ecp_null(&R0);
    *(R0.X.value) = ECP_INF;
    *(R0.Z.value) = ECP_INF;
    ecp_cpy(P, &R1);
    dig_t *kbits = big_to_bin(k, &lenght);
    dig_t *bit = kbits;

    for (int i = lenght; i > 0; i--, bit++) {
        if (*bit) {
            ecp_add(curve, &R0, &R1, p, pn, &t1);
            ecp_cpy(&t1, &R0);
            ecp_double(curve, &R1, p, pn, &t1);
            ecp_cpy(&t1, &R1);
        }
        else {
            ecp_add(curve, &R0, &R1, p, pn, &t1);
            ecp_cpy(&t1, &R1);
            ecp_double(curve, &R0, p, pn, &t1);
            ecp_cpy(&t1, &R0);
        }
    }

    ecp_cpy(&R0, R);
    free(kbits);
}
