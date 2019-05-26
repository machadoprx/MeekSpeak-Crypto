#include "newbignum.h"

big_t*
big_new()
{
	big_t *t = malloc(sizeof(struct _big_t));
	t->sign = false;
	return t;
}

void
big_free(big_t *a)
{
	if (a == NULL) {
		return;
	}

	free(a);
}

static int
big_get_lnt(const big_t *a)
{
	int k = MAXDIGITS - 1;

	while (k >= 0 && a->value[k] == 0) {
		k--;
	}

	return k;
}

char*
big_to_bin(const big_t *a)
{
	if (a == NULL) {
		return NULL;
	}

	char *bin = malloc(512);
	char *p = bin;
	int k = big_get_lnt(a);
	memset(bin, 0, 512);

	for (int i = k; i >= 0; --i) {	
		for (uint32_t mask = 0x80000000u; mask > 0; mask >>= 1) {
			*p++ = (mask & a->value[i]) ? '1' : '0';
		}
	}

	return bin;
}

void
big_null(big_t *a)
{
	if (a == NULL) {
		return;
	}

	memset(a->value, 0, WORDSSIZE);
	a->sign = false;
}

void
big_cpy(const big_t *a, big_t *r)
{
	if (a == NULL || r == NULL) {
		return;
	}

	memcpy(r->value, a->value, WORDSSIZE);
	r->sign = a->sign; 
}

static uint32_t
bin_to_int(const char *num)
{
	uint32_t r = 0;

	for (int i = 0; i < BASEBITS; i++) {
		r = (num[i] == '1') ? (r << 1) ^ 1 : r << 1;
	}

	return r;
}

int 
big_gth(const big_t *a, const big_t *b)
{
	if (a->value == NULL || b->value == NULL) {
		return 0;
	}

	if (a->sign == false && b->sign == true) {
		return 2;
	}

	else if (a->sign == b->sign) {

		if (a->sign == false) {
			for (int i = MAXDIGITS - 1; i >= 0; i--) {

				if (a->value[i] < b->value[i]) {
					return 0;
				}

				else if (a->value[i] > b->value[i]) {
					return 2;
				}

			}

		}

		else{
			for (int i = MAXDIGITS - 1; i >= 0; i--) {

				if (a->value[i] > b->value[i]) {
					return 0;
				}

				else if (a->value[i] < b->value[i]) {
					return 2;
				}
				
			}

		}
		return 1;
	}
	return 0;
}

int 
big_gth_uns(const big_t *a, const big_t *b)
{
	if (a->value == NULL || b->value == NULL) {
		return 0;
	}

	for (int i = MAXDIGITS - 1; i >= 0; i--) {

		if (a->value[i] < b->value[i]) {
			return 0;
		}

		else if (a->value[i] > b->value[i]) {
			return 2;
		}
	}

	return 1;
}

char*
padd_str(const char *str)
{
	char *mask = malloc(sizeof(char) * 512);
	memset(mask, '0', sizeof(char) * 512);
	int j = strlen(str) - 1;

	for (int i = 511; j >= 0; --i) {
		mask[i] = str[j--];
	}

	return mask;
}

void
bin_to_big(const char *src, big_t *r)
{
	if (src == NULL || r == NULL) {
		return;
	}

	big_null(r);
	char *num = padd_str(src);
	int j;

	for (int i = 0; i < MAXDIGITS; ++i) {
		char digit[BASEBITS];
		for (int j = BASEBITS * i, m = 0; j < BASEBITS * (i + 1); j++, m++) {
			digit[m] = num[j];
		}
		r->value[(MAXDIGITS - 1) - i] = bin_to_int(digit);
	}

	free(num);
}

int
big_legendre_symbol(const big_t *a, const big_t *b, big_t *A, const big_t *R, const big_t *beta, const big_t *Rm, const big_t *u, const big_t *bk_minus, const big_t *bk_plus, const big_t *bk_plus_minus)
{
	if (a == NULL || b == NULL || A == NULL || R == NULL || beta == NULL || Rm == NULL || u == NULL || bk_minus == NULL || bk_plus == NULL || bk_plus_minus == NULL) {
		return -2;
	}

	big_t t;
	big_t r;
	big_rst(b, &t);
	big_mont_pow(a, &t, b, A, R, beta, Rm, u, bk_minus, bk_plus, bk_plus_minus, &r);
	return (r.value[0] > 1) ? -1 : r.value[0];
}

void
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

	if (big_gth_uns(r, b) >= EQUAL) {
		big_sub(r, b, &t1);
		big_cpy(&t1, r);
	}
}

void
big_mont_pow(const big_t *a, const big_t *b, const big_t *p, big_t *A, const big_t *R, const big_t *beta, const big_t *Rm, const big_t *u, const big_t *bk_minus, const big_t *bk_plus, const big_t *bk_plus_minus, big_t *r)
{
	if (a == NULL || b == NULL || p == NULL || A == NULL || R == NULL || beta == NULL || Rm == NULL || u == NULL || bk_minus == NULL || bk_plus == NULL || bk_plus_minus == NULL) {
		return;
	}

	big_t xn;
	big_t t;
	char *bin_b = big_to_bin(b);
	int k = 1;
	big_mul(a, R, &t);
	big_barrett_mod(&t, p, u, bk_minus, bk_plus, bk_plus_minus, &xn);

	while (k < strlen(bin_b) && bin_b[k] == '0') {
		k++;
	}

	for (int i = k; i < strlen(bin_b); ++i) {
		big_mul(A, A, &t);
		big_mont(&t, p, Rm, beta, A);
		if (bin_b[i] == '1') {
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
	if (a == NULL || b == NULL || r == NULL) {
		return;
	}

	if (a->sign != b->sign) {
		big_t a1;
		big_t b1;
		big_cpy(a, &a1);
		big_cpy(b, &b1);
		a1.sign = false;
		b1.sign = false;

		if (a->sign == false) {
			big_sub(&a1, &b1, r);
		}
		else {
			big_sub(&b1, &a1, r);
		}
		return;
	}

	big_null(r);
	uint64_t w, carry = 0;

	for (int i = 0; i < MAXDIGITS; ++i) {
		w = carry + (uint64_t) a->value[i] + (uint64_t) b->value[i];
		r->value[i] = w & BASEM;
		carry = w >> 32;
	}

	r->sign = a->sign ? true : false;
}

void
big_sub(const big_t *a, const big_t *b, big_t *r)
{
	if (a == NULL || b == NULL || r == NULL) {
		return;
	}

	if (a->sign != b->sign) {
		big_t a1;
		big_t b1;
		big_cpy(a, &a1);
		big_cpy(b, &b1);
		a1.sign = b1.sign = false;
		big_sum(&a1, &b1, r);
		r->sign = a->sign ? true : false;
		return;
	}

	if (a->sign == true) {
		big_t a1;
		big_t b1;
		big_cpy(a, &a1);
		big_cpy(b, &b1);
		a1.sign = b1.sign = false;
		big_sub(&b1, &a1, r);
		return;
	}

	uint64_t w, borrow = 0;
	big_null(r);
	
	if (big_gth_uns(a, b) > LESS) {

		for (int i = 0; i < MAXDIGITS; ++i) {

			if (a->value[i] < (b->value[i] + borrow)) {
				w = a->value[i] + BASE;
				w = w - b->value[i] - borrow;
				borrow = 1;
			}

			else {
				w = a->value[i] - b->value[i] - borrow;
				borrow = 0;
			}

			r->value[i] = w & BASEM;
		}

		r->sign = false;
	}

	else {

		for (int i = 0; i < MAXDIGITS; ++i) {

			if (b->value[i] < (a->value[i] + borrow)) {
				w = b->value[i] + BASE;
				w = w - a->value[i] - borrow;
				borrow = 1;
			}

			else {
				w = b->value[i] - a->value[i] - borrow;
				borrow = 0;
			}

			r->value[i] = w & BASEM;
		}

		r->sign = true;
	}
}

void
big_mul(const big_t *a, const big_t *b, big_t *r)
{
	if (a == NULL || b == NULL || r == NULL) {
		return;
	}

	big_null(r);
	uint64_t uv;
	uint32_t u = 0, v = 0;

	for (int i = 0; i < MAXDIGITS; ++i) {
		u = 0;
		for (int j = 0; j < MAXDIGITS; ++j) {
			uv = r->value[i + j] + ((uint64_t) a->value[i] * (uint64_t) b->value[j]) + u;
			u = uv >> BASEBITS;
			v = uv & BASEM;
			r->value[i + j] = v;
		} 
	}

	r->sign = (a->sign ^ b->sign);
}

bool 
big_eql(const big_t *a, const big_t *b)
{
	if (a == NULL || b == NULL) {
		return false;
	}

	if (a->sign != b->sign) {
		return false;
	}

	for (int i = MAXDIGITS - 1; i >= 0; i--) {
		if (a->value[i] != b->value[i]) {
			return false;
		}
	}

	return true;
}

void
big_and(const big_t *a, const big_t *b, big_t *r)
{
	if (a == NULL || b == NULL || r == NULL) {
		return;
	}

	big_null(r);
	int n = big_get_lnt(a);
	int m = big_get_lnt(b);
	int g = n >= m ? m : n;

	if (a->sign == false) {
		for (int i = 0; i <= g; i++) {
			r->value[i] = a->value[i] & b->value[i];
		}
	}

	else {
		for (int i = 0; i <= g; i++) {
			r->value[i] = (a->value[i] ^ b->value[i]);
		}

		if (r->value[0] < BASEM) {
			r->value[0]++;
		}
		
		else {
			big_t t;
			big_t one;
			big_null(&one);
			one.value[0] = 1;
			big_sum(&one, r, &t);
			big_cpy(&t, r);
		}
	}
}

void
big_rst_word(const big_t *a, int n, big_t *r)
{
	if (a == NULL || r == NULL || n == 0) {
		return;
	}

	for (int i = 0; i < MAXDIGITS; i++) {
		r->value[i] = a->value[i + n];
	}

	r->sign = a->sign;
}

void
big_rst(const big_t *a, big_t *r)
{
	if (a == NULL || r == NULL) {
		return;
	}

	big_null(r);
	int lsb = 0;

	for (int i = MAXDIGITS - 1; i >= 0; --i) {
		r->value[i] = lsb ? (a->value[i] | 0x100000000) >> 1 : a->value[i] >> 1;
		lsb = a->value[i] & 1;
	}

	r->sign = a->sign;
}

void
big_lst_word(const big_t *a, int n, big_t *r)
{
	if (a == NULL || r == NULL || n == 0) {
		return;
	}

	big_null(r);
	for (int i = 0; i < MAXDIGITS; i++) {
		r->value[i + n] = a->value[i];
	}

	r->sign = a->sign;
}

void
big_lst(const big_t *a, big_t *r)
{
	if (a == NULL || r == NULL) {
		return;
	}

	uint64_t t, carry = 0;
	big_null(r);

	for (int i = 0; i < MAXDIGITS; ++i) {
		t = (a->value[i] << 1) + carry;
		r->value[i] = t & BASEM;
		carry = t >> BASEBITS;
	}

	r->sign = a->sign;
}

void
big_to_hex(const big_t *a)
{
	if (a == NULL) {
		return;
	}

	int n = big_get_lnt(a);
	fputs("0x", stdout);

	for (int i = n; i >= 0; --i) {
		a->value[i] > 0xFFFFFFF ? printf("%lx", a->value[i]) : printf("%08lx", a->value[i]);
	}

	fputs("\n", stdout);
}

void
big_mod(const big_t *a, const big_t *p, big_t *r)
{
	if (a == NULL || r == NULL) {
		return;
	}
	
	big_t t;
	big_null(r);
	big_cpy(a, r);

	if (r->sign == false){
		while (big_gth_uns(r, p) > 0) {
			big_sub(r, p, &t);
			big_cpy(&t, r);
		}
	}

	else {
		while (r->sign == true) {
			big_sum(r, p, &t);
			big_cpy(&t, r);
		}
	}
}

void
big_barrett_mod(const big_t *a, const big_t *p, const big_t *u, const big_t *bk_minus, const big_t *bk_plus, const big_t *bk_plus_minus, big_t *r)
{
	if (a == NULL || r == NULL){
		return;
	}

	big_t t1, t2, t3;
	big_null(r);
	
	big_rst_word(a, 7, &t1);
	big_mul(u, &t1, &t2);
	big_rst_word(&t2, 9, &t1);

	big_and(a, bk_plus_minus, &t2);
	big_mul(p, &t1, &t3); 
	big_and(&t3, bk_plus_minus, &t1);
	big_sub(&t2, &t1, r);

	if (r->sign == true) {
		big_sum(r, bk_plus, &t1);
		big_cpy(&t1, r);
	}

	while (big_gth_uns(r, p) >= EQUAL) {
		big_sub(r, p, &t1);
		big_cpy(&t1, r);
	}

}

bool
big_odd(const big_t *a)
{
	if (a == NULL) {
		return false;
	}

	return (a->value[0] & 1);
}

void
big_mod_inv(const big_t *a, const big_t *b, big_t *r)
{
	if (a == NULL || b == NULL || r == NULL) {
		return;
	}

	if (big_gth_uns(a, b)) {
		return;
	}

	big_t u, v, x1, x2, one, t; 
	big_null(r);
	big_null(&t);
	big_cpy(a, &u);
	big_cpy(b, &v);
	big_null(&x1);
	big_null(&x2);
	big_null(&one);
	x1.value[0] = one.value[0] = 1;

	while (!big_eql(&u, &one) && !big_eql(&v, &one)) {

		while (!big_odd(&u)) {
			big_rst(&u, &t);
			big_cpy(&t, &u);
			if (big_odd(&x1)) {
				big_sum(&x1, b, &t);
				big_cpy(&t, &x1);
			}
			big_rst(&x1, &t);
			big_cpy(&t, &x1);
		}

		while (!big_odd(&v)) {
			big_rst(&v, &t);
			big_cpy(&t, &v);
			if (big_odd(&x2)) {
				big_sum(&x2, b, &t);
				big_cpy(&t, &x2);
			}
			big_rst(&x2, &t);
			big_cpy(&t, &x2);
		}

		if (big_gth(&u, &v) >= EQUAL) {
			big_sub(&u, &v, &t);
			big_cpy(&t, &u);
			big_sub(&x1, &x2, &t);
			big_cpy(&t, &x1);
		}

		else {
			big_sub(&v, &u, &t);
			big_cpy(&t, &v);
			big_sub(&x2, &x1, &t);
			big_cpy(&t, &x2);
		}
		
	}

	big_eql(&u, &one) ? big_mod(&x1, b, r) : big_mod(&x2, b, r);
}