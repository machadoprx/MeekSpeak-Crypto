#include "newbignum.h"

typedef struct _ecp_t{
    big_t *x, *y;
}ecp_t;

typedef struct _ec_t{
    ecp_t *G;
    big_t *A;
}ec_t;