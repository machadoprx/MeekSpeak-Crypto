#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#define DIGIT_SIZE 32
#define BASE 0x100000000u
#define WORD_LENGHT 32 * sizeof(uint64_t)
//#define MAX_UINT64 18446744073709551615
static const char *P224 = "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";

struct _big_t{
	uint64_t *value;
	char *bin;
	int bits_lenght;
	bool sign;
};

typedef struct _big_t big_t;

void big_sub(const big_t *, const big_t *, big_t *);
void big_sum(const big_t *, const big_t *, big_t *);
void big_cpy(const big_t *a, big_t *r);
void big_free(big_t *a);
big_t* big_new();
int big_gth(const big_t *a, const big_t *b);
int big_gth_uns(const big_t *a, const big_t *b);
void bin_to_big(const char *src, big_t *r);
void big_mul(const big_t *a, const big_t *b, big_t *r);
bool big_eql(const big_t *a, const big_t *b);
void big_rst(const big_t *a, big_t *r);
void big_cpy(const big_t *a, big_t *r);
void big_to_hex(const big_t *a);
void big_fst_p224_mod(const big_t *a, big_t *r);
bool big_odd(const big_t *a);
void big_mod_inv(const big_t *a, const big_t *b, void(*rdc)(const big_t*, big_t*), big_t *r);
void big_pow(const big_t *a, const big_t *b, const big_t *c, void(*rdc)(const big_t*, big_t*), big_t *r);