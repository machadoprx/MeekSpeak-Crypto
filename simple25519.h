#include "newbignum.h"

#define GX_25519 9
#define GZ_25519 1
#define GA_25519 486662
static char GY_25519[] = "20ae19a1b8a086b4e01edd2c7748d14c923d4d7e6d7c61b229e9c5a27eced3d9";

typedef struct _ecp_t{
    big_t x, y;
}ecp_t;

typedef struct _ec_t{
    ecp_t *G;
    big_t A;
}ec_t;

ecp_t*      ecp_new();
ec_t*       ec_init_c25519();
void        ecp_null(ecp_t *);

void        ecp_add(ec_t *, ecp_t *, ecp_t *, big_t *, big_t *, ecp_t *);
void        ecp_double(ec_t *, ecp_t *, big_t *, big_t *, ecp_t *);