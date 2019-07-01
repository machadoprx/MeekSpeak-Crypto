#include "simple25519.h"

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
    char *Gy = GY_25519;
    curve->G = ecp_new();
    curve->G->x.value[0] = GX_25519;
    big_null(&curve->A);
    curve->A.value[0] = GA_25519;
    hex_to_big(Gy, &curve->G->y);
    return curve;
}

void
ecp_null(ecp_t *P)
{
    memset(P->x.value, 0, BIG_WORDS_SIZE);
    memset(P->y.value, 0, BIG_WORDS_SIZE);
    P->x.sign = false;
    P->y.sign = false;
}

void
ecp_add(ec_t *curve, ecp_t *P, ecp_t *Q, big_t *p, big_t *pn, ecp_t *R)
{
    big_t t1, t2, t3, t4, commonx, commony, commonx2, commony2, commonA;

    big_sub(&Q->y, &P->y, &t4);
    big_fastmod_25519(&t4, p, pn, &t3);
    big_cpy(&t3, &commony);

    big_mul(&t3, &t3, &t1);
    big_fastmod_25519(&t1, p, pn, &t2); // t2 = ((y2 - y1)^2) % p
    big_cpy(&t2, &commony2);

    big_sub(&Q->x, &P->x, &t4);
    big_fastmod_25519(&t4, p, pn, &t1);
    big_cpy(&t1, &commonx);

    big_mul(&t1, &t1, &t3);
    big_fastmod_25519(&t3, p, pn, &t4); // t4 = ((x2 - x1)^2) % p
    big_cpy(&t4, &commonx2);

    big_mod_inv(&t4, p, &t1);
    big_mul(&t1, &t2, &t3);
    big_fastmod_25519(&t3, p, pn, &t4);

    big_sum(&curve->A, &P->x, &t1);
    big_sum(&t1, &Q->x, &t2);
    big_cpy(&t2, &commonA);

    big_sub(&t4, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &R->x);
    // y3
    big_sum(&P->x, &commonA, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_mul(&t2, &commony, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mod_inv(&commonx, p, &t1);
    big_mul(&t1, &t2, &t3);
    big_fastmod_25519(&t3, p, pn, &t1); // first step
    
    big_mul(&commony2, &commony, &t2);
    big_fastmod_25519(&t2, p, pn, &t3); // 1.5

    big_mul(&commonx2, &commonx, &t2);
    big_fastmod_25519(&t2, p, pn, &t4);
    big_mod_inv(&t4, p, &t2);
    
    big_mul(&t2, &t3, &t4);
    big_fastmod_25519(&t4, p, pn, &t2); // second
    big_sum(&t2, &P->y, &t3);

    big_sub(&t1, &t3, &t2);
    big_fastmod_25519(&t2, p, pn, &R->y);
}

void
ecp_double(ec_t *curve, ecp_t *P, big_t *p, big_t *pn, ecp_t *R)
{
    big_t t1, t2, t3, t4, l, lsqr, commonA;

    big_mul(&P->x, &P->x, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_sum(&t2, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &t3);
    big_sum(&t3, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &t3); // milestone

    big_mul(&curve->A, &P->x, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_sum(&t2, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_null(&t4);
    t4.value[0] = 1ull;
    big_sum(&t4, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_sum(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, pn, &t2); // lnum

    big_sum(&P->y, &P->y, &t3);
    big_fastmod_25519(&t3, p, pn, &t1); 
    big_mod_inv(&t1, p, &t3);
    big_mul(&t3, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &l); // l

    big_mul(&l, &l, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_cpy(&t2, &lsqr);

    big_sub(&t2, &curve->A, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_sub(&t2, &P->x, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_sub(&t2, &P->x, &t1);
    big_fastmod_25519(&t1, p, pn, &R->x); // x

    big_sum(&P->x, &commonA, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&t2, &l, &t1);
    big_fastmod_25519(&t1, p, pn, &t2); // milestone t2

    big_mul(&lsqr, &l, &t1);
    big_fastmod_25519(&t1, p, pn, &t3);

    big_sum(&t3, &P->y, &t1);
    big_sub(&t2, &t1, &t3);
    big_fastmod_25519(&t3, p, pn, &R->y);

}