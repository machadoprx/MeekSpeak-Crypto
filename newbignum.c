#include "newbignum.h"
#include <string.h>
#define DIGIT_SIZE 32
#define WORD_SIZE 4294967296

uint64_t
bin_to_int(char *num)
{
	uint64_t res = 0;
	for (int i = 0; i < strlen(num); ++i){
		res = res << 1;
		if(num[i] == '1')
			res = res | 1;
	}
	return res;
}

char*
padd_str(char *str)
{
	char *mask = (char*)malloc(sizeof(char) * DIGIT_SIZE * DIGIT_SIZE);
	memset(mask, '0', DIGIT_SIZE * DIGIT_SIZE);
	int j = strlen(str) - 1;
	for (int i = (DIGIT_SIZE * DIGIT_SIZE) - 1; j >= 0; --i){
		mask[i] = str[j--];
	}
	return mask;
}

uint64_t*
bin_to_big(char *source)
{
	char *num = padd_str(source);
	int digits = DIGIT_SIZE;
	uint64_t *big = (uint64_t*)malloc(sizeof(uint64_t) * digits);
	for (int i = 0; i < digits; ++i){
		char *digit = (char*)malloc(sizeof(char) * DIGIT_SIZE);
		for (int j = DIGIT_SIZE * i, m = 0; j < DIGIT_SIZE * (i + 1); ++j){
			digit[m++] = num[j];
		}
		big[(digits - 1) - i] = bin_to_int(digit);
		free(digit);
	}
	return big;
}

uint64_t*
add_big(uint64_t *a, uint64_t *b)
{
	int t = 0;
	while(a[t] || b[t]){
		t++;
	}
	uint64_t *c = (uint64_t*)malloc(sizeof(uint64_t) * DIGIT_SIZE);
	int e = 0;
	uint64_t w = a[0] + b[0];
	c[0] = w % WORD_SIZE;
	e = (int)(w / WORD_SIZE);
	for (int i = 1; i < t; ++i){
		w = e + a[i] + b[i];
		c[i] = w % WORD_SIZE;
		e = (int)(w / WORD_SIZE);
	}
	return c;
}

int
main(int argc, char const *argv[])
{
	char *num_1 = "1010111111111111110110100011101011110111111111111000101001110101100010100111010011111100101011101101000011010111111110101011101010";
	uint64_t *big_1 = bin_to_big(num_1);
	uint64_t *big_2 = add_big(big_1, big_1);
	free(big_1);
	free(big_2);
	return 0;
}