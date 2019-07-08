#include "../bn/bn.h"

#define GX_25519 9ull
#define GZ_25519 1ull
#define GA_25519 486662ull
#define ECP_INF  666ull

typedef struct _ecp_t{
    big_t X, Z;
}ecp_t;

typedef struct _ec_t{
    ecp_t *G;
    big_t A;
}ec_t;

ecp_t*      ecp_new();
ec_t*       ec_init_c25519();
void        ecp_null(ecp_t *);

void        ecp_add(ec_t *, ecp_t *, ecp_t *, big_t *, ecp_t *);
void        ecp_double(ec_t *, ecp_t *, big_t *, ecp_t *);
void        ecp_mul(ec_t *, ecp_t *, big_t *, big_t *, ecp_t *);
void        ecp_get_affine(ecp_t *, big_t *, big_t *);