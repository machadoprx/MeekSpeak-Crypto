#include "simple25519.h"

ecp_t*
ecp_new()
{
    ecp_t *T = malloc(sizeof(ecp_t));
    T->x = big_new();
    T->y = big_new();
    return T;
}

ec_t*
ec_init()
{
    char Gy[] = "20ae19a1b8a086b4e01edd2c7748d14c923d4d7e6d7c61b229e9c5a27eced3d9";
    ec_t *t = malloc(sizeof(ec_t));
    t->G = ecp_new();
    t->G->x->value[0] = 9;
    hex_to_big(Gy, t->G->y);
    t->A = big_new();
    t->A->value[0] = 486662;
    return t;
}

void
ec_free(ec_t *a)
{
    if (a != NULL) {
        return;
    }

    big_free(a->G->x);
    big_free(a->G->y);
    big_free(a->A);
    free(a);
}

void
ecp_free(ecp_t *A)
{
    if (A != NULL) {
        return;
    }

    big_free(A->x);
    big_free(A->y);
    free(A);
}

void
ecp_null(ecp_t *A)
{
    memset(A->x->value, 0, BIG_WORDS_SIZE);
    memset(A->y->value, 0, BIG_WORDS_SIZE);
    A->x->sign = false;
    A->y->sign = false;
}

void
ecp_add(ec_t *curve, ecp_t *P, ecp_t *Q, big_t *p, big_t *pn, ecp_t *R)
{
    big_t t1, t2, t3, t4, commonx, commony, commonx2, commony2, commonA;

    // x3
    big_sub(Q->y, P->y, &t4);
    big_fastmod_25519(&t4, p, pn, &t3);
    big_cpy(&t3, &commony);

    big_mul(&t3, &t3, &t1);
    big_fastmod_25519(&t1, p, pn, &t2); // t2 = ((y2 - y1)^2) % p
    big_cpy(&t2, &commony2);

    big_sub(Q->x, P->x, &t4);
    big_fastmod_25519(&t4, p, pn, &t1);
    big_cpy(&t1, &commonx);

    big_mul(&t1, &t1, &t3);
    big_fastmod_25519(&t3, p, pn, &t4); // t4 = ((x2 - x1)^2) % p
    big_cpy(&t4, &commonx2);

    big_mod_inv(&t4, p, &t1);
    big_mul(&t1, &t2, &t3);
    big_fastmod_25519(&t3, p, pn, &t4);

    big_sum(curve->A, P->x, &t1);
    big_sum(&t1, Q->x, &t2);
    big_cpy(&t2, &commonA);

    big_sub(&t4, &t2, &t1);
    big_fastmod_25519(&t1, p, pn, R->x);
    // y3
    big_sum(P->x, &commonA, &t1);
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
    big_sum(&t2, P->y, &t3);

    big_sub(&t1, &t3, &t2);
    big_fastmod_25519(&t2, p, pn, R->y);
}