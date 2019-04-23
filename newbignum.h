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

struct _big_t{
	uint32_t *value;
	int word_lenght;
};

struct _field_t{
	int bit_lenght;
};
typedef struct _big_t big_t;