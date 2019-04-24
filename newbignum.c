#include "newbignum.h"

uint64_t
bin_to_int(const char *num)
{
	uint64_t res = 0;
	for (int i = 0; i < DIGIT_SIZE; ++i){
		res = res << 1;
		if(num[i] == '1')
			res = res | 1;
	}
	return res;
}

int 
ge_than_big(const uint64_t *a, const uint64_t *b)
{
	for (int i = DIGIT_SIZE - 1; i >= 0; i--){
		if (a[i] < b[i]){
			return 0;
		}
		else if(a[i] > b[i]) return 2;
	}
	return 1;
}

char*
padd_str(const char *str)
{
	char *mask = malloc(sizeof(char) * DIGIT_SIZE * DIGIT_SIZE);
	memset(mask, '0', sizeof(char) * DIGIT_SIZE * DIGIT_SIZE);
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
	uint64_t *big = malloc(sizeof(uint64_t) * DIGIT_SIZE);
	for (int i = 0; i < DIGIT_SIZE; ++i){
		char *digit = malloc(sizeof(char) * DIGIT_SIZE);
		for (int j = DIGIT_SIZE * i, m = 0; j < DIGIT_SIZE * (i + 1); ++j){
			digit[m++] = num[j];
		}
		big[(DIGIT_SIZE - 1) - i] = bin_to_int(digit);
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
	uint64_t *c = malloc(WORD_LENGHT);
	int e = 0;
	uint64_t w = a[0] + b[0];
	c[0] = w % BASE;
	e = (int)(w / BASE);
	for (int i = 1; i < t; ++i){
		w = e + a[i] + b[i];
		c[i] = w % BASE;
		e = (int)(w / BASE);
	}
	if(e == 1){
		c[t] += 1;
	}
	return c;
}

uint64_t*
sub_big(const uint64_t *a, const uint64_t *b)
{
	int t = DIGIT_SIZE;
	uint64_t *c = malloc(WORD_LENGHT);
	int e = 0;
	int64_t w = 0;
	w = a[0] - b[0];
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
	uint64_t *c = malloc(WORD_LENGHT);
	memset(c, 0, WORD_LENGHT);
	uint64_t temp;
	uint64_t hi = 0, lo = 0;
	for (int i = 0; i < t; ++i){
		hi = 0;
		for (int j = 0; j < t; ++j){
			temp = c[i + j] + (a[i] * b[j]) + hi;
			hi = (temp >> 32) & 0xFFFFFFFF;
			lo = temp & 0xFFFFFFFF;
			c[i + j] = lo;
		}
		c[i + t] = hi;
	}
	return c;
}

bool 
equal_big(const uint64_t *a, const uint64_t *b)
{
	for (int i = DIGIT_SIZE - 1; i >= 0; i--)
		if (a[i] != b[i])
			return false;
	return true;
}

uint64_t*
rshift_big(const uint64_t *a)
{
	uint64_t *zero = bin_to_big("0");
	if(equal_big(a, zero)) return zero;
	int i = DIGIT_SIZE - 1;
	uint64_t *c = malloc(WORD_LENGHT);
	int lsb = 0;
	while(i >= 0){
		c[i] = a[i] >> 1;
		if(lsb == 1){
			c[i] = (a[i] | 0x100000000) >> 1;
		}
		lsb = a[i] & 1;
		i--;
	}
	free(zero);
	return c;
}

void
big_to_hex(const uint64_t *a)
{
	printf("0x");
	for (int i = DIGIT_SIZE - 1; i >= 0; --i){
		if(a[i] > 0xFFFFFFF)
			printf("%lx", a[i]);
		else if(a[i] == 0)
			printf("000000000");
		else if(a[i] <= 0xFFFFFFF)
			printf("0%lx", a[i]);
	}
	printf("\n");
}

uint64_t*
fast_p224_mod_big(const uint64_t *a, const uint64_t *p)
{
	uint64_t *s_1 = malloc(WORD_LENGHT);
	uint64_t *s_2 = malloc(WORD_LENGHT);
	uint64_t *s_3 = malloc(WORD_LENGHT);
	uint64_t *s_4 = malloc(WORD_LENGHT);
	uint64_t *s_5 = malloc(WORD_LENGHT);
	memset(s_1, 0, WORD_LENGHT);
	memset(s_2, 0, WORD_LENGHT);
	memset(s_3, 0, WORD_LENGHT);
	memset(s_4, 0, WORD_LENGHT);
	memset(s_5, 0, WORD_LENGHT);
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
	int sign = 0;
	uint64_t *sum_1 = sum_big(s_1, s_2);
	uint64_t *sum_2 = sum_big(sum_1, s_3);
	uint64_t *sub_1 = sub_big(sum_2, s_4);
	uint64_t *res = sub_big(sub_1, s_5);
	
	while(ge_than_big(res, p)){
		uint64_t *tmp;
		tmp = sub_big(res, p);
		memcpy(res, tmp, WORD_LENGHT);
		free(tmp);
	}

	free(s_1);
	free(s_2);
	free(s_3);
	free(s_4);
	free(s_5);
	free(sum_1);
	free(sum_2);
	free(sub_1);
	return res;
}

uint64_t *mod_inv_big(uint64_t *a, uint64_t *p)
{
	// verify if a < p
	uint64_t *u = malloc(WORD_LENGHT);
	memcpy(u, a, WORD_LENGHT);
	uint64_t *v = malloc(WORD_LENGHT);
	memcpy(v, p, WORD_LENGHT);
	uint64_t *x_1 = bin_to_big("1");
	uint64_t *x_2 = bin_to_big("0");
	uint64_t *one = bin_to_big("1");
	int x_1_sign = 0;
	int x_2_sign = 0;
	while(!equal_big(u, one) && !equal_big(v, one)){
		while((u[0] & 1) == 0){
			uint64_t *tmp;
			tmp = rshift_big(u);
			memcpy(u, tmp, WORD_LENGHT);
			memset(tmp, 0, WORD_LENGHT);
			free(tmp);
			if((x_1[0] & 1) == 0){
				tmp = rshift_big(x_1);
				memcpy(x_1, tmp, WORD_LENGHT);
				memset(tmp, 0, WORD_LENGHT);
				free(tmp);
			}
			else{
				// x_1 = x_1 + p
				if(x_1_sign == 1){
					// x_1 = (-x_1) + p
					if(ge_than_big(p, x_1) > 0){
						tmp = sub_big(p, x_1);
						x_1_sign = 0;
					}
					else{
						tmp = sub_big(x_1, p);
						x_1_sign = 1;
					}
				}
				else{
					tmp = sum_big(x_1, p);
				}
				memcpy(x_1, tmp, WORD_LENGHT);
				free(tmp);
				tmp = rshift_big(x_1);
				memcpy(x_1, tmp, WORD_LENGHT);
				free(tmp);
			}
		}
		while((v[0] & 1) == 0){
			uint64_t *tmp;
			tmp = rshift_big(v);
			memcpy(v, tmp, WORD_LENGHT);
			free(tmp);
			if((x_2[0] & 1) == 0){
				tmp = rshift_big(x_2);
				memcpy(x_2, tmp, WORD_LENGHT);
				free(tmp);
			}
			else{
				if(x_2_sign == 1){
					if(ge_than_big(p, x_2) > 0){
						tmp = sub_big(p, x_2);
						x_2_sign = 0;
					}
					else{
						tmp = sub_big(x_2, p);
						x_2_sign = 1;
					}
				}
				else{
					tmp = sum_big(p, x_2);
				}
				memcpy(x_2, tmp, WORD_LENGHT);
				memset(tmp, 0, WORD_LENGHT);
				free(tmp);
				tmp = rshift_big(x_2);
				memcpy(x_2, tmp, WORD_LENGHT);
				memset(tmp, 0, WORD_LENGHT);
				free(tmp);
			}
		}
		if(ge_than_big(u, v) > 0){
			uint64_t *tmp = sub_big(u, v);
			memcpy(u, tmp, WORD_LENGHT);
			memset(tmp, 0, WORD_LENGHT);
			free(tmp);
			// x_1 = +x_1 - +x_2
			if(x_1_sign == 1 && x_2_sign == 0){
				// operation (-x_1) - (x_2)
				// x_1 = -x_1 - x_2
				tmp = sum_big(x_1, x_2);
				x_1_sign = 1;
			}
			else if(x_1_sign == 0 && x_2_sign == 1){
				// operation (x_1) - (-x_2)
				// x_1 = x_1 + x_2
				tmp = sum_big(x_1, x_2);
				x_1_sign = 0;
			}
			else{
				// operation (x_1) - (x_2)
				// x_1 = x_1 - x_2
				if(ge_than_big(x_1, x_2) > 0){
					tmp = sub_big(x_1, x_2);
					x_1_sign = 0;
				}
				else{
					tmp = sub_big(x_2, x_1);
					x_1_sign = 1;
				}
			}
			memcpy(x_1, tmp, WORD_LENGHT);
			memset(tmp, 0, WORD_LENGHT);
			free(tmp);
		}
		else{
			int sign_t;
			uint64_t *tmp = sub_big(v, u);
			memcpy(v, tmp, WORD_LENGHT);
			memset(tmp, 0, WORD_LENGHT);
			free(tmp);
			// x_2 = x_2 - x_1
			if(x_1_sign == 1 && x_2_sign == 0){
				// operation (x_2) - (-x_1)
				// x_2 = x_1 + x_2
				tmp = sum_big(x_1, x_2);
				x_2_sign = 0;
			}
			else if(x_1_sign == 0 && x_2_sign == 1){
				// operation (-x_2) - (x_1)
				// x_2 = -x_1 - x_2
				tmp = sum_big(x_1, x_2);
				x_2_sign = 1;
			}
			else{
				// operation (x_2) - (x_1)
				// x_2 = x_2 - x_1
				if(ge_than_big(x_2, x_1) >= 1){
					tmp = sub_big(x_2, x_1);
					x_2_sign = 0;
				}
				else{
					tmp = sub_big(x_1, x_2);
					x_2_sign = 1;
				}
			}
			memcpy(x_2, tmp, WORD_LENGHT);
			memset(tmp, 0, WORD_LENGHT);
			free(tmp);
		}
	}
	uint64_t *res;
	if(equal_big(u, one)){
		// frees
		res = fast_p224_mod_big(x_1, p);
		free(x_1);
		free(x_2);
		free(one);
		free(u);
		free(v);
		return res;
	}
	else{
		res = fast_p224_mod_big(x_2, p);
		free(x_1);
		free(x_2);
		free(one);
		free(u);
		free(v);
		return res;
	}
}

int
main(int argc, char const *argv[])
{
	uint64_t *p224 = bin_to_big("11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001");
	uint64_t *a = bin_to_big("1001100011010101111100011100101110100101010110001011111110001111001101");
	uint64_t *sum = mod_inv_big(a, p224);
	big_to_hex(sum);
	free(a);
	free(p224);
	free(sum);
	return 0;
}