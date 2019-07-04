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
    char GY_25519[] = "20ae19a1b8a086b4e01edd2c7748d14c923d4d7e6d7c61b229e9c5a27eced3d9";
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
    big_t t1, t2, t3, t4, common;

    big_sub(&P->y, &Q->y, &t4);
    big_fastmod_25519(&t4, p, pn, &t3);

    big_sub(&P->x, &Q->x, &t4);
    big_fastmod_25519(&t4, p, pn, &t1);

    big_mod_inv(&t1, p, &t4);
    big_mul(&t3, &t4, &t2);
    big_fastmod_25519(&t2, p, pn, &t1);
    big_cpy(&t1, &common);

    big_mul(&t1, &t1, &t2);
    big_fastmod_25519(&t2, p, pn, &t1);

    big_sub(&t1, &curve->A, &t2);
    big_fastmod_25519(&t2, p, pn, &t1);
    
    big_sub(&t1, &P->x, &t2);
    big_fastmod_25519(&t2, p, pn, &t1);

    big_sub(&t1, &Q->x, &t2);
    big_fastmod_25519(&t2, p, pn, &R->x);

    // y3
    big_sub(&P->x, &R->x, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_mul(&t2, &common, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);

    big_sub(&t2, &P->y, &t1);
    big_fastmod_25519(&t1, p, pn, &R->y);
}

void
ecp_double(ec_t *curve, ecp_t *P, big_t *p, big_t *pn, ecp_t *R)
{
    big_t t1, t2, t3, t4, l, lsqr;

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
    (*t4.value) = 1ull;
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

    big_sum(&P->x, &P->x, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_sum(&t2, &P->x, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_sum(&t2, &curve->A, &t1);
    big_fastmod_25519(&t1, p, pn, &t2);
    big_mul(&l, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, &t2); // keep t2

    big_mul(&lsqr, &l, &t1);
    big_fastmod_25519(&t1, p, pn, &t3); // aqui

    big_sub(&t2, &t3, &t1);
    big_fastmod_25519(&t1, p, pn, &t3);
    big_sub(&t3, &P->y, &t1);
    big_fastmod_25519(&t1, p, pn, &R->y);

}

void
ecp_mul(ec_t *curve, ecp_t *P, big_t *k, big_t *p, big_t *pn, ecp_t *R)
{
    int lenght = 0;
    ecp_t Q, T1;
    dig_t *kbits = big_to_bin(k, &lenght);
    dig_t *bit = kbits + 1;

    big_cpy(&P->x, &Q.x);
    big_cpy(&P->y, &Q.y);
    lenght--;

    while (lenght-- > 0) {

        ecp_double(curve, &Q, p, pn, &T1);
        big_cpy(&T1.x, &Q.x);
        big_cpy(&T1.y, &Q.y);

        if (*(bit++)) {
            ecp_add(curve, &Q, P, p, pn, &T1);
            big_cpy(&T1.x, &Q.x);
            big_cpy(&T1.y, &Q.y);
        }
    }

    big_cpy(&Q.x, &R->x);
    big_cpy(&Q.y, &R->y);
    free(kbits);

}