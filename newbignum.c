#include "newbignum.h"
#include <string.h>
#define DIGIT_SIZE 32
#define BASE 4294967296

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
sum_big(uint64_t *a, uint64_t *b)
{
	int t = 0;
	while(a[t] || b[t]){
		t++;
	}
	uint64_t *c = (uint64_t*)malloc(sizeof(uint64_t) * DIGIT_SIZE);
	int e = 0;
	uint64_t w = a[0] + b[0];
	c[0] = w % BASE;
	if(w > BASE){
		e = 1;
	}
	for (int i = 1; i < t; ++i){
		w = e + a[i] + b[i];
		c[i] = w % BASE;
		if(w > BASE){
			e = 1;
		}
		else e = 0;
	}
	for (int i = 0; i < 2 * t; ++i)
	{
		printf("%lu\n", c[i]);
	}
	return c;
}

uint64_t*
sub_big(uint64_t *a, uint64_t *b)
{
	int t = 0;
	while(a[t] || b[t]){
		t++;
	}
	uint64_t *c = (uint64_t*)malloc(sizeof(uint64_t) * DIGIT_SIZE);
	int e = 0;
	uint64_t w = a[0] - b[0];
	if(w < 0){
		w = w + BASE;
		e = 1;
	}
	c[0] = w % BASE;
	for (int i = 1; i < t; ++i){
		w = a[i] - b[i] - e;
		if(w < 0){
			w = w + BASE;
			e = 1;
		}
		else e = 0;
		c[i] = w % BASE;
	}
	return c;
}



uint64_t*
mul_big(uint64_t *a, uint64_t *b)
{
	int t = 0;
	while(a[t] || b[t]){
		t++;
	}
	uint64_t *c = (uint64_t*)malloc(sizeof(uint64_t) * DIGIT_SIZE);
	memset(c, 0, DIGIT_SIZE);
	uint32_t hi = 0, lo = 0;
	uint64_t res = 0;
	for (int i = 0; i < t; ++i){
		hi = 0;
		for (int j = 0; j < t; ++j){
			// uint64_t 0xFFFFFFFFFFFFFFFF
			res = c[i + j] + (a[i] * b[j]) + hi;
			hi = (res >> 32) & 0xFFFFFFFF;
			lo = res & 0xFFFFFFFF;
			c[i + j] = lo;
		}
		c[i + t] = hi;
	}
	return c;
}

int
main(int argc, char const *argv[])
{
	char *num_1 = "1010111111111111110110100011101011110111111111111000101001110101100010100111010011111100101011101101000011010111111110101011101010";
	char *num_2 = "1111111111111111110110101011101011110111111111111000101001110101100010100111010011111100101011101101000011010111111110101011101011";
	uint64_t *big_1 = bin_to_big(num_1);
	uint64_t *big_3 = bin_to_big(num_2);
	uint64_t *big_2 = mul_big(big_3, big_1);
	free(big_1);
	free(big_2);
	free(big_3);
	return 0;
}