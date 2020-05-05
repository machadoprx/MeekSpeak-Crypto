#ifndef ECC_25519_H
#define ECC_25519_H
#include "../bn/bn.h"

#define GX_25519 9u
#define GZ_25519 1u
#define GA_25519 486662u

typedef struct _ecp_t{
    big_t X, Z;
}ecp_t;

typedef struct _ec_t{
    ecp_t G;
    big_t A;
}ec_t;

#define ecp_null(P) memset(((ecp_t*)P)->X.value, 0, BIG_WORDS_SIZE);  \
                    memset(((ecp_t*)P)->Z.value, 0, BIG_WORDS_SIZE);  \
                    ((ecp_t*)P)->X.sign = false;                      \
                    ((ecp_t*)P)->Z.sign = false;

#define ecp_cpy(P, R)   memcpy(((ecp_t*)R)->X.value, ((ecp_t*)P)->X.value, BIG_WORDS_SIZE); \
                        memcpy(((ecp_t*)R)->Z.value, ((ecp_t*)P)->Z.value, BIG_WORDS_SIZE); \
                        ((ecp_t*)R)->X.sign = ((ecp_t*)P)->X.sign;                          \
                        ((ecp_t*)R)->Z.sign = ((ecp_t*)P)->Z.sign;

  
#define ec_init_c25519(curve)   big_null(&curve.A);                \
                                ecp_null(&curve.G);                \
                                *(curve.G.X.value) = GX_25519;     \
                                *(curve.G.Z.value) = GZ_25519;     \
                                *(curve.A.value) = GA_25519;

void    ecp_add     (ec_t *, ecp_t *, ecp_t *, big_t *, ecp_t *);
void    ecp_dbl     (ec_t *, ecp_t *, big_t *, ecp_t *);
void    ecp_mul     (ec_t *, ecp_t *, big_t *, big_t *, ecp_t *);
void    ecp_mul_cst (ec_t *, ecp_t *, big_t *, big_t *, ecp_t *);
void    ecp_get_afn (ecp_t *, big_t *, big_t *);
#endif