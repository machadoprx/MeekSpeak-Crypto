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
big_get_lnt(big_t *a)
{
	int k = MAX_DIGITS - 1;
	uint_fast32_t *ap = a->value + k;

	while (*ap-- == 0) {
		k--;
	}

	return k;
}

char*
big_to_bin(big_t *a)
{
	if (a == NULL) {
		return NULL;
	}

	char *bin = malloc(512);
	char *p = bin;
	int k = big_get_lnt(a);	
	uint_fast32_t *ap;
	memset(bin, 0, 512);

	for (ap = a->value + k; ap >= &a->value[0]; ap--) {
		for (uint_fast32_t mask = 0x80000000u; mask > 0; mask >>= 1) {
			*p++ = (mask & (*ap)) ? '1' : '0';
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

	memset(a->value, 0, WORDS_SIZE);
	a->sign = false;
}

void
big_cpy(big_t *a, big_t *r)
{
	if (a == NULL || r == NULL) {
		return;
	}

	memcpy(r->value, a->value, WORDS_SIZE);
	r->sign = a->sign; 
}

static uint_fast32_t
bin_to_int(char *num)
{
	uint_fast32_t r = 0;
	char *p = num;

	while (p <= &num[31]) {
		r = (*p++ == '1') ? (r << 1) ^ 1 : r << 1;
	}

	return r;
}

int 
big_gth(big_t *a, big_t *b)
{
	if (a->value == NULL || b->value == NULL) {
		return 0;
	}

	if (a->sign == false && b->sign == true) {
		return 2;
	}

	else if (a->sign == b->sign) {

		if (a->sign == false) {
			for (int i = MAX_DIGITS - 1; i >= 0; i--) {

				if (a->value[i] < b->value[i]) {
					return 0;
				}

				else if (a->value[i] > b->value[i]) {
					return 2;
				}

			}

		}

		else{
			for (int i = MAX_DIGITS - 1; i >= 0; i--) {

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
big_gth_uns(big_t *a, big_t *b)
{
	if (a->value == NULL || b->value == NULL) {
		return 0;
	}

	uint_fast32_t *ap = a->value + (MAX_DIGITS - 1);
    uint_fast32_t *bp = b->value + (MAX_DIGITS - 1);

	for (;ap >= &a->value[0]; ap--, bp--) {

		if (*ap < *bp) {
			return 0;
		}

		else if (*ap > *bp) {
			return 2;
		}
	}

	return 1;
}

static char*
padd_str(char *str)
{
	char *mask = malloc(sizeof(char) * 512);
	char *p = str + (strlen(str) - 1);
	char *m = mask + 511;
	memset(mask, '0', sizeof(char) * 512);

	while (*p) {
		*m-- = *p--;
	}

	return mask;
}

void
bin_to_big(char *src, big_t *r)
{
	if (src == NULL || r == NULL) {
		return;
	}

	char *num = padd_str(src);
	char *nump = num;
	char digit[DIGIT_BITS];
	uint_fast32_t *rp = r->value + (MAX_DIGITS - 1);
	big_null(r);

	for (; rp >= &r->value[0]; rp--) {
		char *digp = digit;

		while (digp <= &digit[31]) {
			*digp++ = *nump++;
		}

		*rp = bin_to_int(digit);
	}

	free(num);
}

int
big_legendre_symbol(big_t *a, big_t *b, big_t *A, big_t *R, big_t *beta, big_t *Rm, big_t *u, big_t *bk_minus, big_t *bk_plus, big_t *bk_plus_minus)
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
big_mont(big_t *a, big_t *b, big_t *Rm, big_t *beta, big_t *r)
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
big_mont_pow(big_t *a, big_t *b, big_t *p, big_t *A, big_t *R, big_t *beta, big_t *Rm, big_t *u, big_t *bk_minus, big_t *bk_plus, big_t *bk_plus_minus, big_t *r)
{
	if (a == NULL || b == NULL || p == NULL || A == NULL || R == NULL || beta == NULL || Rm == NULL || u == NULL || bk_minus == NULL || bk_plus == NULL || bk_plus_minus == NULL) {
		return;
	}

	big_t xn;
	big_t t;
	char *bin_b = big_to_bin(b);
	char *bit = bin_b;
	big_mul(a, R, &t);
	big_barrett_mod(&t, p, u, bk_minus, bk_plus, bk_plus_minus, &xn);

	while (*bit == '0') {
		bit++;
	}

	while (*bit) {
		big_mul(A, A, &t);
		big_mont(&t, p, Rm, beta, A);
		if (*bit++ == '1') {
			big_mul(A, &xn, &t);
			big_mont(&t, p, Rm, beta, A);
		}
	}

	big_mont(A, p, Rm, beta, r);
	free(bin_b);
}

void
big_sum(big_t *a, big_t *b, big_t *r)
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
	uint_fast64_t w, carry = 0;
	uint_fast32_t *ap = a->value, *bp = b->value, *rp = r->value;

	for (; ap <= &a->value[MAX_DIGITS - 1]; ap++, bp++, rp++) {
		w = carry + (*ap) + (*bp);
		(*rp) = w & BASE_M;
		carry = w >> 32;
	}

	r->sign = a->sign ? true : false;
}

void
big_sub(big_t *a, big_t *b, big_t *r)
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

	uint_fast64_t w, borrow = 0;
	uint_fast32_t *ap = a->value, *bp = b->value, *rp = r->value;
	big_null(r);
	
	if (big_gth_uns(a, b) > LESS) {

		for (; ap <= &a->value[MAX_DIGITS - 1]; ap++, bp++, rp++) {

			if (*ap < (*bp + borrow)) {
				w = (*ap) + BASE - (*bp) - borrow;
				borrow = 1;
			}

			else {
				w = (*ap) - (*bp) - borrow;
				borrow = 0;
			}

			(*rp) = w & BASE_M;
		}

		r->sign = false;
	}

	else {

		for (; ap <= &a->value[MAX_DIGITS - 1]; ap++, bp++, rp++) {

			if (*bp < (*ap + borrow)) {
				w = (*bp) + BASE - (*ap) - borrow;
				borrow = 1;
			}

			else {
				w = (*bp) - (*ap) - borrow;
				borrow = 0;
			}

			(*rp) = w & BASE_M;
		}

		r->sign = true;
	}
}

void
big_mul(big_t *a, big_t *b, big_t *r)
{
	if (a == NULL || b == NULL || r == NULL) {
		return;
	}

	big_null(r);
	uint_fast64_t uv;
	uint_fast32_t u, v;
	uint_fast32_t *ap = a->value, *bp;
	uint_fast32_t *rp = r->value, *trp;

	for (; ap <= &a->value[MAX_DIGITS - 1]; ap++, rp++) {
		u = 0;
		for (trp = rp, bp = b->value; bp <= &b->value[MAX_DIGITS - 1]; bp++, trp++) {
			uv = (*trp) + ((*ap) * (*bp)) + u;
			u = uv >> DIGIT_BITS;
			v = uv & BASE_M;
			(*trp) = v;
		} 
	}

	r->sign = (a->sign ^ b->sign);
}

/*void
big_sqr(big_t *a, big_t *r)
{
	if (a == NULL || r == NULL) {
		return;
	}

	big_null(r);
	uint_fast64_t uv, u, tmpx, *tmpt;
	int i, j, n;
	n = big_get_lnt(a);

	for (i = 0; i <= n; ++i) {
		uv = r->value[2 * i] + (a->value[i] * a->value[i]);
		r->value[i + i] = uv & BASE_M;
		u = uv >> DIGIT_BITS;
		tmpx = a->value[i];
		tmpt = r->value + (2 * i + 1);
		for (j = i + 1; j <= n; ++j) {
			uv = tmpx * a->value[j];
			uv = (*tmpt) + uv + uv + u;
			*tmpt++ = uv & BASE_M;
			u = uv >> DIGIT_BITS;
		} 
		while (u != 0) {
			uv = (*tmpt) + u;
			*tmpt++ = uv & BASE_M;
			u = uv >> DIGIT_BITS;
		}
	}

	r->sign = false;
}*/

bool 
big_eql(big_t *a, big_t *b)
{
	if (a == NULL || b == NULL) {
		return false;
	}

	if (a->sign != b->sign) {
		return false;
	}

	uint_fast32_t *ap = a->value;
	uint_fast32_t *bp = b->value;

	for (; ap <= &a->value[MAX_DIGITS - 1]; ap++, bp++) {
		
		if (*ap != *bp) {
			return false;
		}
	}

	return true;
}

void
big_and(big_t *a, big_t *b, big_t *r)
{
	if (a == NULL || b == NULL || r == NULL) {
		return;
	}

	big_null(r);
	int n = big_get_lnt(a);
	int m = big_get_lnt(b);
	int g = n >= m ? m : n;
	uint_fast32_t *ap = a->value;
	uint_fast32_t *bp = b->value;
	uint_fast32_t *rp = r->value;

	if (a->sign == false) {
		for (; rp <= &r->value[g]; rp++, ap++, bp++) {
			*rp = (*ap) & (*bp);
		}
	}

	else {
		for (; rp <= &r->value[g]; rp++, ap++, bp++) {
			*rp = (*ap) ^ (*bp);
		}

		if ((*r->value) < BASE_M) {
			(*r->value)++;
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
big_rst_word(big_t *a, int n, big_t *r)
{
	if (a == NULL || r == NULL || n == 0) {
		return;
	}
	
	big_null(r);
	uint_fast32_t *ap = a->value + n;
	uint_fast32_t *rp = r->value;
	
	for (; rp <= &r->value[MAX_DIGITS - 1]; rp++, ap++) {
		*rp = *ap;
	}

	r->sign = a->sign;
}

void
big_rst(big_t *a, big_t *r)
{
	if (a == NULL || r == NULL) {
		return;
	}

	big_null(r);
	int lsb = 0;
	uint_fast32_t *ap = a->value + (MAX_DIGITS - 1);
	uint_fast32_t *rp = r->value + (MAX_DIGITS - 1);

	for (; rp >= &r->value[0]; ap--, rp--) {
		*rp = lsb ? ((*ap) | 0x100000000) >> 1 : (*ap) >> 1;
		lsb = (*ap) & 1;
	}

	r->sign = a->sign;
}

void
big_lst_word(big_t *a, int n, big_t *r)
{
	if (a == NULL || r == NULL || n == 0) {
		return;
	}

	uint_fast32_t *ap = a->value;
	uint_fast32_t *rp = r->value + n;
	big_null(r);
	
	for (; rp <= &r->value[MAX_DIGITS - 1]; rp++, ap++) {
		*rp = *ap;
	}

	r->sign = a->sign;
}

void
big_lst(big_t *a, big_t *r)
{
	if (a == NULL || r == NULL) {
		return;
	}

	uint_fast64_t t, carry = 0;
	uint_fast32_t *ap = a->value;
	uint_fast32_t *rp = r->value;
	big_null(r);

	for (; rp <= &r->value[MAX_DIGITS - 1]; rp++, ap++) {
		t = (*ap << 1) + carry;
		*rp = t & BASE_M;
		carry = t >> DIGIT_BITS;
	}

	r->sign = a->sign;
}

void
big_to_hex(big_t *a)
{
	if (a == NULL) {
		return;
	}

	int n = big_get_lnt(a);
	uint_fast32_t *ap = a->value + n;
	fputs("0x", stdout);

	for (; ap >= &a->value[0]; --ap) {
		*ap > 0xFFFFFFF ? printf("%lx", *ap) : printf("%08lx", *ap);
	}

	fputs("\n", stdout);
}

void
big_mod(big_t *a, big_t *p, big_t *r)
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
big_barrett_mod(big_t *a, big_t *p, big_t *u, big_t *bk_minus, big_t *bk_plus, big_t *bk_plus_minus, big_t *r)
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
big_odd(big_t *a)
{
	if (a == NULL) {
		return false;
	}

	return (*(a->value) & 1);
}

void
big_mod_inv(big_t *a, big_t *b, big_t *r)
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