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

int
big_get_lnt(big_t *a)
{
	register int k = MAX_DIGITS - 1;

	while (a->value[k] == 0) {
		k--;
	}

	return k;
}

char*
big_to_bin(big_t *a)
{
	char *bin = malloc(512);
	register int k = big_get_lnt(a);	
	memset(bin, 0, 512);

	int j = 0;
	for (int i = k; i >= 0; i--) {
		for (register dig_t mask = 0x80000000u; mask > 0; mask >>= 1, j++) {
			bin[j] = (mask & (a->value[i])) ? '1' : '0';
		}
	}

	return bin;
}

void
big_null(big_t *a)
{
	memset(a->value, 0, WORDS_SIZE);
	a->sign = false;
}

void
big_cpy(big_t *a, big_t *r)
{
	memcpy(r->value, a->value, WORDS_SIZE);
	r->sign = a->sign; 
}

dig_t
bin_to_int(char *num)
{
	register dig_t r = 0;
	register int i = 0;
	
	while (i < DIGIT_BITS) {
		r = (num[i] == '1') ? (r << 1) ^ 1 : r << 1;
		i++;
	}

	return r;
}

int 
big_gth(big_t *a, big_t *b)
{
	if (a->sign == b->sign) {

		return a->sign ? big_gth_uns(b, a) : big_gth_uns(a, b);

	}
	return a->sign ? 0 : 2;
}

int 
big_gth_uns(big_t *a, big_t *b)
{
	int n = big_get_lnt(a);
	int m = big_get_lnt(b);
	
	if (n != m) {
		return n > m ? 2 : 0;
	}

	register int i = n;

	while (i >= 0 && a->value[i] == b->value[i]) {
		i--;
	}

	if (i == 0 && (a->value[i] == b->value[i])) {
		return 1;
	}

	return (a->value[i] < b->value[i]) ? 0 : 2;
}

static void
padd_str(char *str, char *dest, int padd)
{
	register int i = strlen(str) - 1, j = padd - 1;
	memset(dest, '0', sizeof(char) * padd);

	
	while (i >= 0) {
		dest[j] = str[i];
		j--; i--;
	}
}

void
bin_to_big(char *src, big_t *r)
{
	char num[512];
	padd_str(src, num, 512);
	char digit[DIGIT_BITS];
	register int i, j, m = 0;;
	big_null(r);

	for (i = MAX_DIGITS - 1; i >= 0; i--) {
		j = 0;

		while (j < DIGIT_BITS) {
			digit[j] = num[m];
			j++; m++;
		}

		r->value[i] = bin_to_int(digit);
	}
}

void
hex_to_big(char *src, big_t *r)
{
	char num[128];
	padd_str(src, num, 128);
	char digit[9];
	register int i, j, m = 0;
	big_null(r);

	for (i = MAX_DIGITS - 1; i >= 0; i--) {

		j = 0;

		while (j < 8) {
			digit[j] = num[m];
			j++; m++;
		}
		num[8] = '\0';

		r->value[i] = strtoull(digit, NULL, 16);
	}
}

int
big_legendre_symbol(big_t *a, big_t *b, big_t *A, big_t *R, big_t *beta, big_t *Rm, big_t *pn)
{
	big_t t;
	big_t r;
	big_rst(b, &t);
	big_mont_pow(a, &t, b, A, R, beta, Rm, pn, &r);
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
big_mont_pow(big_t *a, big_t *b, big_t *p, big_t *A, big_t *R, big_t *beta, big_t *Rm, big_t *pn, big_t *r)
{
	big_t xn;
	big_t t;
	char *bin_b = big_to_bin(b);
	big_mul(a, R, &t);
	big_mod2(&t, p, pn, &xn);

	register int k = 0, end = strlen(bin_b);
	while (bin_b[k] == '0') {
		k++;
	}

	while (k < end) {
		big_mul(A, A, &t);
		big_mont(&t, p, Rm, beta, A);
		if (bin_b[k] == '1') {
			big_mul(A, &xn, &t);
			big_mont(&t, p, Rm, beta, A);
		}
		k++;
	}

	big_mont(A, p, Rm, beta, r);
	free(bin_b);
}

void
big_sum(big_t *a, big_t *b, big_t *r)
{
	if (a->sign != b->sign) {
		big_t a1;
		big_t b1;
		big_cpy(a, &a1);
		big_cpy(b, &b1);
		a1.sign = false;
		b1.sign = false;
		a->sign ? big_sub(&b1, &a1, r) : big_sub(&a1, &b1, r);
		return;
	}

	big_null(r);
	int n = big_get_lnt(a), m = big_get_lnt(b);
	bool gth = false;
	register dig_t w, carry = 0;

	if (n < m) {
		int t = n; n = m; m = t;
		gth = true;
	}

	int i = 0;
	for (; i <= m; i++) {
		w = carry + a->value[i] + b->value[i];
		r->value[i] = w & BASE_M;
		carry = w >> DIGIT_BITS;
	}

	if (!gth) {
		while (carry) {
			w = carry + a->value[i];
			r->value[i] = w & BASE_M;
			carry = w >> DIGIT_BITS;
			i++;
		}

		while (i <= n) {
			r->value[i] = a->value[i];
			i++;
		}
	}

	else {
		while (carry) {
			w = carry + b->value[i];
			r->value[i] = w & BASE_M;
			carry = w >> DIGIT_BITS;
			i++;
		}

		while (i <= n) {
			r->value[i] = b->value[i];
			i++;
		}
	}

	r->sign = a->sign ? true : false;
}

void
big_sub(big_t *a, big_t *b, big_t *r)
{
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

	int n = big_get_lnt(a);
	int m = big_get_lnt(b);
	bool gth = false;
	register int64_t w, borrow = 0;
	big_null(r);

	if (big_gth_uns(a, b) == LESS) {
		int t = n; n = m; m = t; m++;
		gth = true;
	}

	int i = 0;
	if (!gth) {

		for (; i <= m; i++) {

			w = a->value[i] - b->value[i] - borrow;	// warning: mix int with uint
			if (w < 0) {
				w += BASE;
				borrow = 1;
			}
			else {
				borrow = 0;
			}
			r->value[i] = w;
		}

		while (borrow) {

			w = a->value[i] - borrow;
			if (w < 0) {
				w += BASE;
				borrow = 1;
			}
			else {
				borrow = 0;
			}
			r->value[i] = w;
			i++;
		}

		while (i <= n) {
			r->value[i] = a->value[i];
			i++;
		}

		r->sign = false;
	}

	else {
		
		for (; i <= m; i++) {

			w = b->value[i] - a->value[i] - borrow;	// warning: mix int with uint
			if (w < 0) {
				w += BASE;
				borrow = 1;
			}
			else {
				borrow = 0;
			}
			r->value[i] = w;
		}

		while (borrow) {

			w = b->value[i] - borrow;
			if (w < 0) {
				w += BASE;
				borrow = 1;
			}
			else {
				borrow = 0;
			}
			r->value[i] = w;
			i++;
		}

		while (i <= n) {
			r->value[i] = b->value[i];
			i++;
		}

		r->sign = true;
	}
}

void
big_mul_nasty(big_t *a, big_t *b, big_t *r)
{
	big_null(r);
	int n = big_get_lnt(a);
	int m = big_get_lnt(b) + 1;
	register dig_t uv, u;
	register int i, j;

	for (i = 0; i <= n; i++) {
		u = 0;
		for (j = 0; j <= m; j++) {
			uv = r->value[i + j] + (a->value[i] * b->value[j]) + u;
			u = uv >> DIGIT_BITS;
			r->value[i + j] = uv & BASE_M;
		} 
	}

	r->sign = (a->sign ^ b->sign);
}

void
big_sqr(big_t *a, big_t *r)
{
	big_null(r);
	int n = big_get_lnt(a);
	register twodig_t uv;
	register int i, j;
	register dig_t u, tmpx;

	for (i = 0; i <= n; i++) {
		
		uv = r->value[i + i] + (a->value[i] * a->value[i]);
		r->value[i + i] = uv & BASE_M;
		u = uv >> DIGIT_BITS;
		tmpx = a->value[i];
		int m = i + i + 1;

		for (j = i + 1; j <= n; j++) {
			uv = tmpx * a->value[j];
			uv = r->value[m] + uv + uv + u;
			r->value[m] = uv & BASE_M;
			u = uv >> DIGIT_BITS;
			m++;
		} 

		while (u != 0) {
			
			uv = r->value[m] + u;
			r->value[m] = uv & BASE_M;
			u = uv >> DIGIT_BITS;
			m++;
		}
	}

	r->sign = false;
}

bool 
big_eql(big_t *a, big_t *b)
{
	if (a->sign != b->sign) {
		return false;
	}

	int n = big_get_lnt(a);
	int m = big_get_lnt(b);
	int g = n >= m ? n : m;
	register int i;

	for (i = 0; i <= g; i++) {
		
		if (a->value[i] != b->value[i]) {
			return false;
		}
	}

	return true;
}

void
big_and(big_t *a, big_t *b, big_t *r)
{
	int n = big_get_lnt(a);
	int m = big_get_lnt(b);
	int g = n >= m ? m : n;
	register int i;
	big_null(r);

	if (a->sign == false) {
		for (i = 0; i <= g; i++) {
			r->value[i] = a->value[i] & b->value[i];
		}
	}

	else {
		for (i = 0; i <= g; i++) {
			r->value[i] = a->value[i] ^ b->value[i];
		}

		if (r->value[0] < BASE_M) {
			r->value[0]++;
		}
		
		else {
			big_t t, one;
			big_null(&one);
			one.value[0] = 1;
			big_sum(&one, r, &t);
			big_cpy(&t, r);
		}
	}
}

void
big_rst_word(big_t *a, int s, big_t *r)
{
	int n = big_get_lnt(a) - s;
	register int i;
	big_null(r);
	
	for (i = 0; i <= n; i++) {
		r->value[i] = a->value[i + s];
	}

	r->sign = a->sign;
}

void
big_rst(big_t *a, big_t *r)
{
	int n = big_get_lnt(a);
	register int i;
	register int lsb = 0;
	big_null(r);
	
	for (i = n; i >= 0; i--) {
		r->value[i] = lsb ? (a->value[i] | BASE) >> 1 : a->value[i] >> 1;
		lsb = a->value[i] & 1;
	}

	r->sign = a->sign;
}

void
big_lst_word(big_t *a, int s, big_t *r)
{
	register int n = big_get_lnt(a) + s;
	register int i;
	big_null(r);
	
	for (i = 0; i <= n; i++) {
		r->value[i + s] = a->value[i];
	}

	r->sign = a->sign;
}

void
big_lst(big_t *a, big_t *r)
{
	int n = big_get_lnt(a) + 1;
	register dig_t t, carry = 0;
	register int i;
	big_null(r);

	for (i = 0; i <= n; i++) {
		t = (a->value[i] << 1) + carry;
		r->value[i] = t & BASE_M;
		carry = t >> DIGIT_BITS;
	}

	r->sign = a->sign;
}

void
big_to_hex(big_t *a)
{
	register int n = big_get_lnt(a);

	fputs("0x", stdout);
	for (; n >= 0; n--) {
		a->value[n] > 0xFFFFFFF ? printf("%lx", a->value[n]) : printf("%08lx", a->value[n]);
	}
	fputs("\n", stdout);
}

void
big_mod(big_t *a, big_t *p, big_t *r)
{
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
big_mul(big_t *a, big_t *b, big_t *r)
{
	dig_t uv = 0, u = 0;
	big_null(r);
	int n = big_get_lnt(a);
	int m = big_get_lnt(b);
	int i, j, g = n + m + 1;
	if (n < m) {
		int t = n;
		n = m; m = t;
	}

	for (i = 0; i <= g; i++) {
		if (i <= n) {
			for (j = 0; j <= i; j++) {
				uv = uv + (a->value[j] * b->value[i - j]);
				u = (uv >> DIGIT_BITS) + u;
				uv = uv & BASE_M;
			}
		}
		else {
			for (j = i - n; j <= n; j++) {
				uv = uv + (a->value[j] * b->value[i - j]);
				u = (uv >> DIGIT_BITS) + u;
				uv = uv & BASE_M;
			}
		}
		r->value[i] = uv & BASE_M;
		uv = u & BASE_M;
		u >>= DIGIT_BITS;
	}

	r->sign = a->sign ^ b->sign;
}

void
big_mod2(big_t *a, big_t *p, big_t *pn, big_t *r)
{
	big_t t1, t2, t3, tmpq, tmpk;
	dig_t lsb;
	big_null(&tmpk);
	big_cpy(a, r);
	tmpk.value[0] = 19;

	while (big_gth_uns(r, p) >= EQUAL) {

		lsb = r->value[8] & 0x80000000ul;
		big_rst_word(r, 8, &t1);
		big_lst(&t1, &tmpq);

		if (lsb) {
			tmpq.value[0]++;
		}

		big_and(r, pn, &t1);
		big_mul(&tmpq, &tmpk, &t2);
		big_sum(&t1, &t2, r);
		
		if (big_gth_uns(r, p) >= EQUAL) {
			
			big_sub(r, p, &t3);
			big_cpy(&t3, r);
		}
	}
}

void
big_barrett_mod(big_t *a, big_t *p, big_t *u, big_t *bk_minus, big_t *bk_plus, big_t *bk_plus_minus, big_t *r)
{
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

void
big_mod_inv(big_t *a, big_t *b, big_t *r)
{
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

		while (!(u.value[0] & 1)) {
			big_rst(&u, &t);
			big_cpy(&t, &u);
			if ((x1.value[0] & 1)) {
				big_sum(&x1, b, &t);
				big_cpy(&t, &x1);
			}
			big_rst(&x1, &t);
			big_cpy(&t, &x1);
		}

		while (!(v.value[0] & 1)) {
			big_rst(&v, &t);
			big_cpy(&t, &v);
			if ((x2.value[0] & 1)) {
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