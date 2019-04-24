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
static const char *P224 = "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";

struct _big_t{
	uint64_t *value;
	int bits_lenght;
	bool sign;
};

typedef struct _big_t big_t;