#include "newbignum.h"

big_t*
big_new()
{
	big_t *t = malloc(sizeof(struct _big_t));
	t->value = malloc(WORD_LENGHT);
	memset(t->value, 0, WORD_LENGHT);
	t->bits_lenght = 0;
	t->sign = false;
	return t;
}

void
big_free(big_t *a)
{
	if(a == NULL) 
		return;
	free(a->value);
	free(a);
}

static void
big_null(big_t *a)
{
	memset(a->value, 0, WORD_LENGHT);
	a->bits_lenght = 0;
	a->sign = false;
}

static uint64_t
bin_to_int(const char *num)
{
	uint64_t r = 0;
	for(int i = 0; i < DIGIT_SIZE; ++i){
		r = r << 1;
		if(num[i] == '1')
			r = r | 1;
	}
	return r;
}

int 
big_gth(const big_t *a, const big_t *b)
{
	if(a->value == NULL || b->value == NULL)
		return 0;
	for(int i = DIGIT_SIZE - 1; i >= 0; i--){
		if(a->value[i] < b->value[i])
			return 0;
		else 
			if(a->value[i] > b->value[i]) 
				return 2;
	}
	return 1;
}

static char*
padd_str(const char *str)
{
	char *mask = malloc(sizeof(char) * DIGIT_SIZE * DIGIT_SIZE);
	memset(mask, '0', sizeof(char) * DIGIT_SIZE * DIGIT_SIZE);
	int j = strlen(str) - 1;
	for(int i = (DIGIT_SIZE * DIGIT_SIZE) - 1; j >= 0; --i){
		mask[i] = str[j--];
	}
	return mask;
}

void
bin_to_big(const char *src, big_t *r)
{
	if(src == NULL || r == NULL)
		return;

	big_null(r);
	char *num = padd_str(src);
	for(int i = 0; i < DIGIT_SIZE; ++i){
		char *digit = malloc(sizeof(char) * DIGIT_SIZE);
		for(int j = DIGIT_SIZE * i, m = 0; j < DIGIT_SIZE * (i + 1); ++j){
			digit[m++] = num[j];
		}
		r->value[(DIGIT_SIZE - 1) - i] = bin_to_int(digit);
		free(digit);
	}
	free(num);
}

void
big_sum(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL)
		return;

	big_null(r);
	int e = 0;
	uint64_t w = a->value[0] + b->value[0];
	r->value[0] = w % BASE;
	e = (int)(w / BASE);
	for(int i = 1; i < DIGIT_SIZE; ++i){
		w = e + a->value[i] + b->value[i];
		r->value[i] = w % BASE;
		e = (int)(w / BASE);
	}
}

void
big_sub(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL)
		return;

	big_null(r);
	int e = 0;
	int64_t w = 0;
	w = a->value[0] - b->value[0];
	if(w < 0){
		w = w + BASE;
		e = 1;
	}
	r->value[0] = w % BASE;
	for(int i = 1; i < DIGIT_SIZE; ++i){
		w = a->value[i] - b->value[i] - e;
		if(w < 0){
			w = w + BASE;
			e = 1;
		}
		else 
			e = 0;
		r->value[i] = w % BASE;
	}
}

void
big_mul(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL)
		return;

	big_null(r);
	int t = DIGIT_SIZE;
	uint64_t temp;
	uint64_t hi = 0, lo = 0;
	for(int i = 0; i < DIGIT_SIZE; ++i){
		hi = 0;
		for(int j = 0; j < t; ++j){
			temp = r->value[i + j] + (a->value[i] * b->value[j]) + hi;
			hi = (temp >> 32) & 0xFFFFFFFF;
			lo = temp & 0xFFFFFFFF;
			r->value[i + j] = lo;
		}
		r->value[i + t] = hi;
	}
}

bool 
big_eql(const big_t *a, const big_t *b)
{
	if(a == NULL || b == NULL)
		return false;

	for(int i = DIGIT_SIZE - 1; i >= 0; i--)
		if (a->value[i] != b->value[i])
			return false;
	return true;
}

void
big_rst(const big_t *a, big_t *r)
{
	if(a == NULL || r == NULL)
		return;

	big_null(r);
	big_t *zero = big_new();
	bin_to_big("0", zero);
	if(big_eql(a, zero)) 
		return;
	big_free(zero);

	int lsb = 0;
	for(int i = DIGIT_SIZE - 1; i >= 0; --i){
		r->value[i] = a->value[i] >> 1;
		if(lsb == 1)
			r->value[i] = (a->value[i] | 0x100000000) >> 1;
		lsb = a->value[i] & 1;
	}
}

void
big_cpy(const big_t *a, big_t *r)
{
	if(a == NULL || r == NULL)
		return;

	memcpy(r->value, a->value, WORD_LENGHT);
	r->bits_lenght = a->bits_lenght;
	r->sign = a->sign;
}

void
big_to_hex(const big_t *a)
{
	printf("0x");
	for(int i = DIGIT_SIZE - 1; i >= 0; --i){
		if(a->value[i] > 0xFFFFFFF)
			printf("%lx", a->value[i]);
		else if(a->value[i] == 0)
			printf("000000000");
		else
			printf("0%lx", a->value[i]);
	}
	printf("\n");
}

void
big_fst_p224_mod(const big_t *a, big_t *r)
{
	if(a == NULL || r == NULL)
		return;

	big_null(r);
	big_t *p = big_new(); bin_to_big(P224, p);
	big_t *s_1;	s_1 = big_new();
	big_t *s_2;	s_2 = big_new();
	big_t *s_3;	s_3 = big_new();
	big_t *s_4;	s_4 = big_new();
	big_t *s_5;	s_5 = big_new();

	int i = 0;
	for(i = 0; i < 7; ++i){
		s_1->value[i] = a->value[i];
		s_4->value[i] = a->value[i + 7];
	}
	for(i = 3; i < 7; ++i)
		s_2->value[i] = a->value[i + 4];
	for(i = 3; i < 6; ++i)
		s_3->value[i] = a->value[i + 8];
	for(i = 0; i < 3; ++i)
		s_5->value[i] = a->value[i + 11];

	big_t *sum_1 = big_new();
	big_sum(s_1, s_2, sum_1);
	big_t *sum_2 = big_new();
	big_sum(sum_1, s_3, sum_2);
	big_t *sub_1 = big_new();
	big_sub(sum_2, s_4, sub_1);
	big_sub(sub_1, s_5, r);

	while(big_gth(r, p)){
		big_t *tmp = big_new();
		big_sub(r, p, tmp);
		big_cpy(tmp, r);
		big_free(tmp);
	}

	big_free(p);
	big_free(s_1); big_free(s_2);
	big_free(s_3); big_free(s_4);
	big_free(s_5); big_free(sum_1);
	big_free(sum_2); big_free(sub_1);
}

bool
big_odd(const big_t *a)
{
	if(a == NULL)
		return false;
	return (a->value[0] & 1);
}

void
big_mod_inv(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL)
		return;
	if(big_gth(a, b))
		return;

	big_null(r);
	big_t *u = big_new(); 
	big_cpy(a, u);
	big_t *v = big_new(); 
	big_cpy(b, v);
	big_t *x1 = big_new(); 
	bin_to_big("1", x1);
	big_t *x2 = big_new();
	bin_to_big("0", x2);
	big_t *one = big_new(); 
	bin_to_big("1", one);
	int x1_sign = 0, x2_sign = 0;

	while(!big_eql(u, one) && !big_eql(v, one)){
		//big_to_hex(x1);
		while(!big_odd(u)){
			big_t *tmp = big_new();
			big_rst(u, tmp);
			big_cpy(tmp, u);
			big_null(tmp);
			if(big_odd(x1)){
				if(x1_sign == 1){
					if(big_gth(b, x1) > 0){
						big_sub(b, x1, tmp);
						x1_sign = 0;
					}
					else{
						big_sub(x1, b, tmp);
						x1_sign = 1;
					}
				}
				else
					big_sum(x1, b, tmp);
				
				big_cpy(tmp, x1);
				big_null(tmp);
			}
			big_rst(x1, tmp);
			big_cpy(tmp, x1);
			big_free(tmp);
		}
		while(!big_odd(v)){
			big_t *tmp = big_new(); 
			big_rst(v, tmp);
			big_cpy(tmp, v);
			big_null(tmp);
			if(big_odd(x2)){
				if(x2_sign == 1){
					if(big_gth(b, x2) > 0){
						big_sub(b, x2, tmp);
						x2_sign = 0;
					}
					else{
						big_sub(x2, b, tmp);
						x2_sign = 1;
					}
				}
				else
					big_sum(x2, b, tmp);

				big_cpy(tmp, x2);
				big_null(tmp);
			}
			big_rst(x2, tmp);
			big_cpy(tmp, x2);
			big_free(tmp);
		}
		if(big_gth(u, v) > 0){
			big_t *tmp = big_new();
			big_sub(u, v, tmp);
			big_cpy(tmp, u);
			big_null(tmp);
			if(x1_sign == 1 && x2_sign == 0){
				big_sum(x1, x2, tmp);
				x1_sign = 1;
			}
			else
				if(x1_sign == 0 && x2_sign == 1){
					big_sum(x1, x2, tmp);
					x1_sign = 0;
				}
			else{
				if(big_gth(x1, x2) > 0){
					big_sub(x1, x2, tmp);
					x1_sign = 0;
				}
				else{
					big_sub(x2, x1, tmp);
					x1_sign = 1;
				}
			}
			big_cpy(tmp, x1);
			big_free(tmp);
		}
		else{
			big_t *tmp = big_new();
			big_sub(v, u, tmp);
			big_cpy(tmp, v);
			big_null(tmp);
			if(x1_sign == 1 && x2_sign == 0){
				big_sum(x1, x2, tmp);
				x2_sign = 0;
			}
			else
				if(x1_sign == 0 && x2_sign == 1){
					big_sum(x1, x2, tmp);
					x2_sign = 1;
				}
			else{
				if(big_gth(x2, x1) > 0){
					big_sub(x2, x1, tmp);
					x2_sign = 0;
				}
				else{
					big_sub(x1, x2, tmp);
					x2_sign = 1;
				}
			}
			big_cpy(tmp, x2);
			big_free(tmp);
		}
	}

	if(big_eql(u, one))
		big_fst_p224_mod(x1, r);
	else
		big_fst_p224_mod(x2, r);

	big_free(x1);
	big_free(x2);
	big_free(one);
	big_free(u);
	big_free(v);
}

int
main(int argc, char const *argv[])
{
	big_t *p224 = big_new();
	bin_to_big("11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001", p224);
	big_t *a = big_new();
	bin_to_big("1001100011010101111100011100101110100101010110001011111110001111001101", a);
	big_t *sum = big_new();
	big_mod_inv(a, p224, sum);
	big_to_hex(sum);
	big_free(a);
	big_free(p224);
	big_free(sum);
	return 0;
}