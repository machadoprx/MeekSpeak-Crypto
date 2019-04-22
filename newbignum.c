#include "newbignum.h"
#include <string.h>
#define DIGIT_SIZE 32
#define BASE 0x100000000u

uint64_t
bin_to_int(const char *num)
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
padd_str(const char *str)
{
	char *mask = malloc(sizeof(char) * DIGIT_SIZE * DIGIT_SIZE);
	memset(mask, '0', DIGIT_SIZE * DIGIT_SIZE);
	int j = strlen(str) - 1;
	for (int i = (DIGIT_SIZE * DIGIT_SIZE) - 1; j >= 0; --i){
		mask[i] = str[j--];
	}
	return mask;
}

uint64_t*
bin_to_big(const char *source)
{
	char *num = padd_str(source);
	int digits = DIGIT_SIZE;
	uint64_t *big = malloc(sizeof(uint64_t) * digits);
	for (int i = 0; i < digits; ++i){
		char *digit = malloc(sizeof(char) * DIGIT_SIZE);
		for (int j = DIGIT_SIZE * i, m = 0; j < DIGIT_SIZE * (i + 1); ++j){
			digit[m++] = num[j];
		}
		big[(digits - 1) - i] = bin_to_int(digit);
		//big[(digits - 1) - i] = strtoull(digit, &n, 2);
		free(digit);
	}
	free(num);
	return big;
}

uint64_t*
sum_big(const uint64_t *a, const uint64_t *b)
{
	int t = DIGIT_SIZE;
	uint64_t *c = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	int e = 0;
	uint64_t w = a[0] + b[0];
	c[0] = w % BASE;
	if (w >= BASE)
		e = 1;
	for (int i = 1; i < t; ++i){
		w = e + a[i] + b[i];
		c[i] = w % BASE;
		if (w >= BASE)
			e = 1;
		else e = 0;
	}
	return c;
}

uint64_t*
sub_big(const uint64_t *a, const uint64_t *b)
{
	int t = DIGIT_SIZE;
	uint64_t *c = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	int e = 0;
	int64_t w = a[0] - b[0];
	//printf("%lu\n", w);
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
mul_big(const uint64_t *a, const uint64_t *b)
{
	int t = DIGIT_SIZE;
	uint64_t *c = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	memset(c, 0, DIGIT_SIZE);
	uint32_t hi = 0, lo = 0;
	uint64_t res = 0;
	for (int i = 0; i < t; ++i){
		hi = 0;
		for (int j = 0; j < t; ++j){
			res = c[i + j] + (a[i] * b[j]) + hi;
			hi = (res >> 32) & 0xFFFFFFFF;
			lo = res & 0xFFFFFFFF;
			c[i + j] = lo;
		}
		c[i + t] = hi;
	}
	return c;
}

uint64_t*
rshift_big(const uint64_t *a)
{
	int i = 0;
	uint64_t *c = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	while(a[i]){
		c[i] = a[i] >> 1;
		if((a[i + 1] & 1) == 1){
			c[i] = (a[i] | 0x100000000) >> 1;
		}
		i++;
	}
	return c;
}

bool ge_than_big(const uint64_t *a, const uint64_t *b)
{
	for (int i = DIGIT_SIZE - 1; i >= 0; i--){
		if (a[i] < b[i]){
			return false;
		}
		else if(a[i] > b[i]) return true;
	}
	return true;
}

uint64_t*
fastred_p224_mod_big(const uint64_t *a, const uint64_t *p)
{
	uint64_t *s_1 = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	uint64_t *s_2 = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	uint64_t *s_3 = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	uint64_t *s_4 = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	uint64_t *s_5 = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	memset(s_1, 0, DIGIT_SIZE);
	memset(s_2, 0, DIGIT_SIZE);
	memset(s_3, 0, DIGIT_SIZE);
	memset(s_4, 0, DIGIT_SIZE);
	memset(s_5, 0, DIGIT_SIZE);
	for (int i = 0; i < 7; ++i){
		s_1[i] = a[i];
	}
	for (int i = 3; i < 7; ++i){
		s_2[i] = a[i + 4];
	}
	for (int i = 3; i < 6; ++i){
		s_3[i] = a[i + 8];
	}
	for (int i = 0; i < 7; ++i){
		s_4[i] = a[i + 7];
	}
	for (int i = 0; i < 3; ++i){
		s_5[i] = a[i + 11];
	}
	uint64_t *sum_1 = sum_big(s_1, s_2);
	uint64_t *sum_2 = sum_big(sum_1, s_3);
	uint64_t *sub_1 = sub_big(sum_2, s_4);
	uint64_t *res = sub_big(sub_1, s_5);
	
	while(ge_than_big(res, p) == true){
		uint64_t *tmp = malloc(sizeof(uint64_t) * DIGIT_SIZE);
		for (int i = 0; i < DIGIT_SIZE; ++i)
			tmp[i] = res[i];
		free(res);
		res = sub_big(tmp, p);
		free(tmp);
	}
	/*
	for (int i = 0; i < DIGIT_SIZE; ++i){
		printf("%d res: %lu p: %lu\n", i, res[i], p[i]);
	}*/

	free(s_1);
	free(s_2);
	free(s_3);
	free(s_4);
	free(s_5);
	free(sum_1);
	free(sum_2);
	free(sub_1);
	// while res > p, res = res - p
	return res;
}

int
main(int argc, char const *argv[])
{
	/*
	char *num_1 = "11010111111111111110110100011101011110111111111111000101001110101100010100111010011111100101011101101000011010111111110101011101011";
	char *num_2 = "1101011111111111111011010001110101111011111111111100010100111010110001010011101001111110010101110110100001101011111111010101110101";
	uint64_t *big_1 = bin_to_big(num_1);
	uint64_t *big_2 = bin_to_big(num_2);
	uint64_t *big_3 = rshift_big(big_1);
	for (int i = 0; i < 5; ++i)
	{
		printf("%d %lu\n", i, big_3[i]);
	}
	free(big_1);
	free(big_2);
	free(big_3);*/
	char *p = "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";
	uint64_t *prime = bin_to_big(p);
	char *test = "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
	uint64_t *test_b = bin_to_big(test);
	uint64_t *res = fastred_p224_mod_big(test_b, prime);
	// res must be: 11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111
	uint64_t *test_bmodp224 = bin_to_big("11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111111111111111111111");
	for (int i = 0; i < DIGIT_SIZE; ++i)
	{
		printf("%d %lu\n", i, test_bmodp224[i]);
	}
	free(res);
	free(test_bmodp224);
	free(prime);
	free(test_b);
	return 0;
}