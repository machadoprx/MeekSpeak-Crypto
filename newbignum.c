#include "newbignum.h"

inline big_t*
big_new()
{
	big_t *t = malloc(sizeof(struct _big_t));
	t->sign = false;
	return t;
}

void
big_free(big_t *a)
{
	if(a == NULL){
		return;
	}
	free(a);
}

static inline int
big_get_lnt(const big_t *a)
{
	int k = MAXDIGITS - 1;
	while(k >= 0 && a->value[k] == 0){
		k--;
	}
	return k;
}

char*
big_to_bin(const big_t *a)
{
	if(a == NULL){
		return NULL;
	}
	char *bin = malloc(512);
	char *p = bin;
	int k = big_get_lnt(a);
	memset(bin, 0, 512);
	for(int i = k; i >= 0; --i){	
		for(uint64_t mask = 0x80000000u; mask > 0; mask >>= 1){
			*p++ = (mask & a->value[i]) ? '1' : '0';
		}
	}
	return bin;
}

inline void
big_null(big_t *a)
{
	if(a == NULL){
		return;
	}
	memset(a->value, 0, WORDSSIZE);
	a->sign = false;
}

inline void
big_cpy(const big_t *a, big_t *r)
{
	if(a == NULL || r == NULL){
		return;
	}
	memcpy(r->value, a->value, WORDSSIZE);
	r->sign = a->sign; 
}

static inline uint64_t
bin_to_int(const char *num)
{
	uint64_t r = 0;
	r = (num[0] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[1] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[2] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[3] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[4] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[5] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[6] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[7] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[8] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[9] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[10] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[11] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[12] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[13] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[14] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[15] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[16] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[17] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[18] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[19] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[20] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[21] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[22] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[23] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[24] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[25] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[26] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[27] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[28] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[29] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[30] == '1') ? (r << 1) ^ 1 : r << 1;
	r = (num[31] == '1') ? (r << 1) ^ 1 : r << 1;
	return r;
}

int 
big_gth(const big_t *a, const big_t *b)
{
	if(a->value == NULL || b->value == NULL){
		return 0;
	}
	if(a->sign == false && b->sign == false){
		for(int i = MAXDIGITS - 1; i >= 0; i--){
			if(a->value[i] < b->value[i]){
				return 0;
			}
			else if(a->value[i] > b->value[i]){
				return 2;
			}
		}
		return 1;
	}
	else if(a->sign == true && b->sign == true){
		for(int i = MAXDIGITS - 1; i >= 0; i--){
			if(a->value[i] > b->value[i]){
				return 0;
			}
			else if(a->value[i] < b->value[i]) {
				return 2;
			}
		}
		return 1;
	}
	else if(a->sign == false && b->sign == true){
		return 2;
	}
	return 0;
}

inline int 
big_gth_uns(const big_t *a, const big_t *b)
{
	if(a->value == NULL || b->value == NULL){
		return 0;
	}
	for(int i = MAXDIGITS - 1; i >= 0; i--){
		if(a->value[i] < b->value[i]){
			return 0;
		}
		else if(a->value[i] > b->value[i]){
			return 2;
		}
	}
	return 1;
}

static inline char*
padd_str(const char *str)
{
	char *mask = malloc(sizeof(char) * 512);
	memset(mask, '0', sizeof(char) * 512);
	int j = strlen(str) - 1;
	for(int i = 511; j >= 0; --i){
		mask[i] = str[j--];
	}
	return mask;
}

inline void
bin_to_big(const char *src, big_t *r)
{
	if(src == NULL || r == NULL){
		return;
	}
	char *num = padd_str(src);
	big_null(r);
	int j;
	for(int i = 0; i < MAXDIGITS; ++i){
		char digit[32];
		j = 32 * i;
		digit[0] = num[j];
		digit[1] = num[j + 1];
		digit[2] = num[j + 2];
		digit[3] = num[j + 3];
		digit[4] = num[j + 4];
		digit[5] = num[j + 5];
		digit[6] = num[j + 6];
		digit[7] = num[j + 7];
		digit[8] = num[j + 8];
		digit[9] = num[j + 9];
		digit[10] = num[j + 10];
		digit[11] = num[j + 11];
		digit[12] = num[j + 12];
		digit[13] = num[j + 13];
		digit[14] = num[j + 14];
		digit[15] = num[j + 15];
		digit[16] = num[j + 16];
		digit[17] = num[j + 17];
		digit[18] = num[j + 18];
		digit[19] = num[j + 19];
		digit[20] = num[j + 20];
		digit[21] = num[j + 21];
		digit[22] = num[j + 22];
		digit[23] = num[j + 23];
		digit[24] = num[j + 24];
		digit[25] = num[j + 25];
		digit[26] = num[j + 26];
		digit[27] = num[j + 27];
		digit[28] = num[j + 28];
		digit[29] = num[j + 29];
		digit[30] = num[j + 30];
		digit[31] = num[j + 31];
		r->value[(MAXDIGITS - 1) - i] = bin_to_int(digit);
	}
	free(num);
}

int
big_legendre_symbol(const big_t *a, const big_t *b, big_t *A, const big_t *R, const big_t *beta, const big_t *Rm, const big_t *u, const big_t *bk_minus, const big_t *bk_plus, const big_t *bk_plus_minus)
{
	if(a == NULL || b == NULL){
		return -2;
	}
	big_t t;
	big_t r;
	uint64_t x;
	big_cpy(b, &t);
	t.value[0] -= 1;
	big_rst(&t, &r);
	big_mont_pow(a, &r, b, A, R, beta, Rm, u, bk_minus, bk_plus, bk_plus_minus, &t);
	x = t.value[0];
	if(x > 1){
		return -1;
	}
	return (int)x;
}

static inline void
big_mont(const big_t *a, const big_t *b, const big_t *Rm, const big_t *beta, big_t *r)
{
	big_t t1;
	big_null(r);
	big_and(a, Rm, &t1);
	big_mul(&t1, beta, r);
	big_and(r, Rm, &t1);
	big_mul(&t1, b, r);
	big_sum(a, r, &t1);
	big_rst_word(&t1, 8, r);
	if(big_gth(r, b) >= EQUAL){
		big_sub(r, b, &t1);
		big_cpy(&t1, r);
	}
}

void
big_mont_pow(const big_t *a, const big_t *b, const big_t *p, big_t *A, const big_t *R, const big_t *beta, const big_t *Rm, const big_t *u, const big_t *bk_minus, const big_t *bk_plus, const big_t *bk_plus_minus, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}

	big_t xn;
	big_t t;
	char *bin_b = big_to_bin(b);
	int k = 1;
	big_mul(a, R, &t);
	big_fst_mod(&t, p, u, bk_minus, bk_plus, bk_plus_minus, &xn);
	while(k < strlen(bin_b) && bin_b[k] == '0'){
		k++;
	}
	for(int i = k; i < strlen(bin_b); ++i){
		big_mul(A, A, &t);
		big_mont(&t, p, Rm, beta, A);
		if(bin_b[i] == '1'){
			big_mul(A, &xn, &t);
			big_mont(&t, p, Rm, beta, A);
		}
	}
	big_mont(A, p, Rm, beta, r);
	free(bin_b);
}

void
big_sum(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}
	if(a->sign != b->sign){
		big_t a1;
		big_cpy(a, &a1);
		a1.sign = false;
		big_t b1;
		big_cpy(b, &b1);
		b1.sign = false;
		if(a->sign == false){
			big_sub(&a1, &b1, r);
		}
		else{
			big_sub(&b1, &a1, r);
		}
		return;
	}
	big_null(r);
	int e = 0;
	uint64_t w = a->value[0] + b->value[0];
	r->value[0] = w & BASEM;
	e = (int)(w >> 32);
	for(int i = 1; i < MAXDIGITS; ++i){
		w = e + a->value[i] + b->value[i];
		r->value[i] = w & BASEM;
		e = (int)(w >> 32);
	}
	if(a->sign == true){
		r->sign = true;
	}
}

void
big_sub(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}
	if(a->sign != b->sign){
		big_t a1;
		big_cpy(a, &a1);
		a1.sign = false;
		big_t b1;
		big_cpy(b, &b1);
		b1.sign = false;
		big_sum(&a1, &b1, r);
		if(a->sign == true){
			r->sign = true;
		}
		return;
	}
	if(a->sign == true){
		big_t a1;
		big_cpy(a, &a1);
		a1.sign = false;
		big_t b1;
		big_cpy(b, &b1);
		b1.sign = false;
		big_sub(&b1, &a1, r);
		return;
	}
	bool gth = false;
	if(big_gth_uns(a, b) > 0){
		gth = true;
	}
	big_null(r);
	int e = 0;
	int64_t w = 0;
	if(gth == true){
		w = a->value[0] - b->value[0];
	}
	else{
		w = b->value[0] - a->value[0];
	}
	if(w < 0){
		w = w + BASE;
		e = 1;
	}
	r->value[0] = w & BASEM;
	for(int i = 1; i < MAXDIGITS; ++i){
		w = a->value[i] - b->value[i] - e;
		if(gth == true){
			w = a->value[i] - b->value[i] - e;
		}
		else{
			w = b->value[i] - a->value[i] - e;
		}
		if(w < 0){
			w = w + BASE;
			e = 1;
		}
		else{
			e = 0;
		}
		r->value[i] = w & BASEM;
	}
	if(gth == true){
		r->sign = false;
	}
	else{
		r->sign = true;
	}
}

void
big_mul(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}

	big_null(r);
	uint64_t temp;
	uint64_t hi = 0, lo = 0;
	for(int i = 0; i < MAXDIGITS; ++i){
		hi = 0;
		for(int j = 0; j < MAXDIGITS; ++j){
			temp = r->value[i + j] + (a->value[i] * b->value[j]) + hi;
			hi = (temp >> 32) & BASEM;
			lo = temp & BASEM;
			r->value[i + j] = lo;
		} 
		//r->value[i + MAXDIGITS - 1] = hi;
	}
	r->sign = (a->sign ^ b->sign);
}

bool 
big_eql(const big_t *a, const big_t *b)
{
	if(a == NULL || b == NULL){
		return false;
	}
	if(a->sign != b->sign){
		return false;
	}

	for(int i = MAXDIGITS - 1; i >= 0; i--){
		if (a->value[i] != b->value[i]){
			return false;
		}
	}
	return true;
}

void
big_and(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}

	big_null(r);
	int n = big_get_lnt(a);
	int m = big_get_lnt(b);
	int g = n >= m ? m : n;
	if(a->sign == false){
		for(int i = 0; i <= g; i++){
			r->value[i] = a->value[i] & b->value[i];
		}
	}
	else{
		for(int i = 0; i <= g; i++){
			r->value[i] = (a->value[i] ^ b->value[i]);
		}
		if(r->value[0] < BASEM){
			r->value[0]++;
		}
		else{
			big_t t;
			big_t one;
			bin_to_big("1", &one);
			big_sum(&one, r, &t);
			big_cpy(&t, r);
		}
	}
}

void
big_rst_word(const big_t *a, uint32_t n, big_t *r)
{
	if(a == NULL || r == NULL || n == 0){
		return;
	}
	r->value[0] = a->value[n];
	r->value[1] = a->value[1 + n];
	r->value[2] = a->value[2 + n];
	r->value[3] = a->value[3 + n];
	r->value[4] = a->value[4 + n];
	r->value[5] = a->value[5 + n];
	r->value[6] = a->value[6 + n];
	r->value[7] = a->value[7 + n];
	r->value[8] = a->value[8 + n];
	r->value[9] = a->value[9 + n];
	r->value[10] = a->value[10 + n];
	r->value[11] = a->value[11 + n];
	r->value[12] = a->value[12 + n];
	r->value[13] = a->value[13 + n];
	r->value[14] = a->value[14 + n];
	r->value[15] = a->value[15 + n];
	r->sign = a->sign;
}

inline void
big_rst(const big_t *a, big_t *r)
{
	if(a == NULL || r == NULL){
		return;
	}
	big_null(r);
	int lsb = 0;
	for(int i = MAXDIGITS - 1; i >= 0; --i){
		r->value[i] = lsb ? (a->value[i] | 0x100000000) >> 1 : a->value[i] >> 1;
		lsb = a->value[i] & 1;
	}
	r->sign = a->sign;
}

void
big_to_hex(const big_t *a)
{
	int n = big_get_lnt(a);
	fputs("0x", stdout);
	for(int i = n; i >= 0; --i){
		if(a->value[i] > 0xFFFFFFF){
			printf("%lx", a->value[i]);
		}
		else{
			printf("%08lx", a->value[i]);
		}
	}
	fputs("\n", stdout);
}

void
big_mod(const big_t *a, const big_t *p, big_t *r)
{
	if(a == NULL || r == NULL){
		return;
	}
	big_null(r);
	big_cpy(a, r);
	big_t tmp;
	if(r->sign == false){
		while(big_gth_uns(r, p) > 0){
			big_sub(r, p, &tmp);
			big_cpy(&tmp, r);
		}
	}
	else{
		while(r->sign == true){
			big_sum(r, p, &tmp);
			big_cpy(&tmp, r);
		}
	}
}

void
big_fst_mod(const big_t *a, const big_t *p, const big_t *u, const big_t *bk_minus, const big_t *bk_plus, const big_t *bk_plus_minus, big_t *r)
{
	if(a == NULL || r == NULL){
		return;
	}
	big_null(r);
	big_t t1;
	big_t t2;
	big_t t3;
	
	big_rst_word(a, 7, &t1);
	big_mul(u, &t1, &t2);
	big_rst_word(&t2, 9, &t1);

	big_and(a, bk_plus_minus, &t2);
	big_mul(p, &t1, &t3); 
	big_and(&t3, bk_plus_minus, &t1);
	big_sub(&t2, &t1, r);

	if(r->sign == true){
		big_sum(r, bk_plus, &t1);
		big_cpy(&t1, r);
	}
	while(big_gth_uns(r, p) >= EQUAL){
		big_sub(r, p, &t1);
		big_cpy(&t1, r);
	}
}

bool
big_odd(const big_t *a)
{
	if(a == NULL){
		return false;
	}
	return (a->value[0] & 1);
}

void
big_mod_inv(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}
	big_null(r);
	if(big_gth(a, b)){
		return;
	}

	big_t u; 
	big_t v; 
	big_t x1; 
	big_t x2;
	big_t one;

	big_cpy(a, &u);
	big_cpy(b, &v);
	bin_to_big("1", &x1);
	bin_to_big("0", &x2);
	bin_to_big("1", &one);

	while(!big_eql(&u, &one) && !big_eql(&v, &one)){
		big_t tmp;
		while(!big_odd(&u)){
			big_rst(&u, &tmp);
			big_cpy(&tmp, &u);
			if(big_odd(&x1)){
				big_sum(&x1, b, &tmp);
				big_cpy(&tmp, &x1);
			}
			big_rst(&x1, &tmp);
			big_cpy(&tmp, &x1);
		}
		while(!big_odd(&v)){
			big_rst(&v, &tmp);
			big_cpy(&tmp, &v);
			if(big_odd(&x2)){
				big_sum(&x2, b, &tmp);
				big_cpy(&tmp, &x2);
			}
			big_rst(&x2, &tmp);
			big_cpy(&tmp, &x2);
		}
		if(big_gth(&u, &v) >= EQUAL){
			big_sub(&u, &v, &tmp);
			big_cpy(&tmp, &u);
			big_sub(&x1, &x2, &tmp);
			big_cpy(&tmp, &x1);
		}
		else{
			big_sub(&v, &u, &tmp);
			big_cpy(&tmp, &v);
			big_sub(&x2, &x1, &tmp);
			big_cpy(&tmp, &x2);
		}
	}
	if(big_eql(&u, &one)){
		big_mod(&x1, b, r);
	}
	else{
		big_mod(&x2, b, r);
	}
}