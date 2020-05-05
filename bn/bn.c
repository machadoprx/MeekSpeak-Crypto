#include "bn.h"

int
big_get_len(big_t *a)
{
	int k = BIG_MAX_DIGITS - 1;
	dig_t *ap = a->value + k;

	while (*ap == 0u && --k > 0) {
		ap--;
	}

	return k;
}

dig_t*
big_to_bin(big_t *a, int *lenght)
{
	int k = big_get_len(a);	
	dig_t *bin = malloc(512 * sizeof(dig_t));
	dig_t *p = bin;
	dig_t *ap = a->value + k, t = *(ap), mask = 0x80000000u;
	memset(bin, 0, 512 * sizeof(dig_t));
	int mswbits = 0;

	while (t > 0) {
		t = t >> 1;
		mswbits++;
	}

	while (!(mask & (*ap))) {
		mask = mask >> 1;
	}

	for (; ap >= a->value; ap--) {
		while (mask != 0u) { //cst time?
			*(p++) = (dig_t)((mask & (*ap)) / mask);
			mask = mask >> 1;
		}
		mask = 0x80000000u;
	}

	*lenght = mswbits + (k * BIG_DIGIT_BITS);

	return bin;
}

dig_t
bin_to_int(char *num)
{
	dig_t r = 0;
	char *p = num, *stop = num + BIG_DIGIT_BITS - 1;

	while (p <= stop) {
		r = (*p++ == '1') ? (r << 1) ^ 1 : r << 1;
	}

	return r;
}

int 
big_gth(big_t *a, big_t *b)
{
	if (a->sign == b->sign) {
		return a->sign ? big_gth_uns(b, a) : big_gth_uns(a, b);
	}
	else {
		return a->sign ? -1 : 1;
	}
}

int
big_gth_uns(big_t *a, big_t *b)
{
	int len_a = big_get_len(a);
	int len_b = big_get_len(b);

	if (len_a != len_b) {
		return (len_a > len_b) ? 1 : -1; 
	}

	for (int i = len_a; i >= 0; i--) {
		if (a->value[i] > b->value[i]) {
			return 1;
		}
		else if (a->value[i] < b->value[i]) {
			return -1;
		}
	}
	return 0;
}

static void
padd_str(char *str, char *dest, int padd)
{
	char *p = str + (strlen(str) - 1);
	char *m = dest + padd - 1;
	memset(dest, '0', sizeof(char) * padd);

	while (*p != 0) {
		*m-- = *p--;
	}
}

void
bin_to_big(char *src, big_t *r)
{
	big_null(r);

	char num[512], digit[BIG_DIGIT_BITS];
	padd_str(src, num, 512);

	char *nump = num;
	dig_t *rp = r->value + (BIG_MAX_DIGITS - 1);
	char *digp, *stop = digit + BIG_DIGIT_BITS - 1;

	for (; rp >= r->value; rp--) {
		digp = digit;

		while (digp <= stop) {
			*(digp++) = *(nump++);
		}

		*rp = bin_to_int(digit);
	}
}

void
hex_to_big(char *src, big_t *r)
{
	big_null(r);

	char num[128], digit[9];
	padd_str(src, num, 128);

	char *nump = num;
	dig_t *rp = r->value + (BIG_MAX_DIGITS - 1);
	char *digp, *stop = digit + 7;

	for (; rp >= r->value; rp--) {
		digp = digit;

		while (digp <= stop) {
			*(digp++) = *(nump++);
		}
		*(digp) = '\0';

		*rp = strtoul(digit, NULL, 16);
	}
}

int
big_lgd_sym(big_t *a, big_t *b)
{
	big_t t, r;
	big_rst(b, &t);
	big_mnt_pow_25519(a, &t, &r);
	return ((*r.value) > 1u) ? -1 : (*r.value);
}

void
big_mnt(big_t *a, big_t *b, big_t *Rm, big_t *beta, big_t *r)
{
	big_null(r);

	big_t t1;
	big_and(a, Rm, &t1);
	big_mul(&t1, beta, r);
	big_and(r, Rm, &t1);
	big_mul(&t1, b, r);
	big_sum(a, r, &t1);
	big_rst_wrd(&t1, 8, r);

	if (big_gth_uns(r, b) >= 0) {
		big_sub(r, b, &t1);
		big_cpy(&t1, r);
	}
}

void
big_mnt_pow_25519(big_t *a, big_t *b, big_t *r)
{
	big_null(r);

	big_t xn, t, p, A, beta, R, Rm;

	uint32_t BETA_25519[] 	= {
								0xD79435E5u, 0x79435E50u, 0x9435E50Du, 0x435E50D7u, 
								0x35E50D79u, 0x5E50D794u, 0xE50D7943u, 0xD0D79435u
	};
	uint32_t RM_25519[] 	= {
								0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 
								0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu
	};

	big_null(&beta);
	big_null(&R);
	big_null(&Rm);
	big_null(&p);
	big_null(&A);

	memcpy(Rm.value, RM_25519, sizeof(dig_t) * 8);
	memcpy(p.value, P25519, sizeof(dig_t) * 8);
	memcpy(beta.value, BETA_25519, sizeof(dig_t) * 8);
	memcpy(p.value, P25519, sizeof(dig_t) * 8);
	R.value[8] = 0x01u;
	A.value[0] = 0x26u;
	beta.sign = true;

	int b_lenght;
	dig_t *bin_b = big_to_bin(b, &b_lenght);
	dig_t *bit = bin_b;
	big_mul(a, &R, &t);
	big_mod_25519(&t, &p, &xn); 

	while (b_lenght-- > 0) {
		big_mul(&A, &A, &t);
		big_mnt(&t, &p, &Rm, &beta, &A);
		if (*(bit++) != 0) { // problem: side channel leak
			big_mul(&A, &xn, &t);
			big_mnt(&t, &p, &Rm, &beta, &A);
		}
	}

	big_mnt(&A, &p, &Rm, &beta, r);
	free(bin_b);
}

void
big_sum(big_t *a, big_t *b, big_t *r)
{
	if (a->sign != b->sign) {
		big_t a1, b1;
		big_cpy(a, &a1);
		big_cpy(b, &b1);
		a1.sign = b1.sign = false;
		a->sign ? big_sub(&b1, &a1, r) : big_sub(&a1, &b1, r);
		return;
	}

	big_null(r);
	int n = big_get_len(a), m = big_get_len(b);
	twodig_t w = 0, carry = 0;
	dig_t *gp, *lp, *rp = r->value;

	if (n >= m) {
		lp = b->value;
		gp = a->value;
	}
	else {
		lp = a->value;
		gp = b->value;
		int t = n; n = m; m = t;
	}
	
	dig_t *stop1 = r->value + n + 1, *stop2 = r->value + m;

	for (; rp <= stop2; lp++, gp++, rp++) {
		w = carry + (uint64_t)(*gp) + (uint64_t)(*lp);
		*rp = (uint32_t)(w & BIG_BASE_M);
		carry = w >> BIG_DIGIT_BITS;
	}

	while (rp <= stop1) {
		w = carry + (uint64_t)(*(gp++));
		*(rp++) = (uint32_t)(w & BIG_BASE_M);
		carry = w >> BIG_DIGIT_BITS;
	}

	r->sign = a->sign ? true : false;
}

void
big_sub(big_t *a, big_t *b, big_t *r)
{
	if (a->sign != b->sign) {
		big_t a1, b1;
		big_cpy(a, &a1);
		big_cpy(b, &b1);
		a1.sign = b1.sign = false;
		big_sum(&a1, &b1, r);
		r->sign = a->sign ? true : false;
		return;
	}

	if (a->sign == true) {
		big_t a1, b1;
		big_cpy(a, &a1);
		big_cpy(b, &b1);
		a1.sign = b1.sign = false;
		big_sub(&b1, &a1, r);
		return;
	}

	int n = big_get_len(a);
	int m = big_get_len(b), t = -1;
	dig_t *gp, *lp, *rp = r->value, borrow = 0;
	big_null(r);

	if (big_gth_uns(a, b) >= 0) {
		gp = a->value;
		lp = b->value;
	}
	else {
		gp = b->value;
		lp = a->value;
		t = n; n = m; m = t; m++;
	}

	uint64_t w = 0;
	dig_t *stop1 = r->value + n, *stop2 = r->value + m, shift = ((8 * sizeof(uint64_t)) - 1);

	for (; rp <= stop2; gp++, lp++, rp++) {

		w = (uint64_t)(*gp) - (uint64_t)(*lp) - borrow;
		borrow = w >> shift;
		*rp = (uint32_t)(w & BIG_BASE_M);
	}

	while (rp <= stop1) {

		w = (uint64_t)(*gp) - borrow;
		borrow = w >> shift;
		*(rp++) = (uint32_t)(w & BIG_BASE_M);
		gp++;
	}

	r->sign = (t == -1) ? false : true;
}

bool 
big_eql(big_t *a, big_t *b)
{
	if (a->sign != b->sign) {
		return false;
	}

	int n = big_get_len(a);
	int m = big_get_len(b);
	int g = n >= m ? n : m;
	dig_t *ap = a->value, *bp = b->value, *stop = a->value + g;

	for (; ap <= stop; ap++, bp++) {
		if (*ap != *bp) {
			return false;
		}
	}

	return true;
}

void
big_and(big_t *a, big_t *b, big_t *r)
{
	big_null(r);

	int n = big_get_len(a);
	int m = big_get_len(b);
	int g = n >= m ? m : n;
	dig_t *ap = a->value, *bp = b->value, *rp = r->value, *stop = r->value + g;

	if (a->sign == false) {
		for (; rp <= stop; rp++, ap++, bp++) {
			*rp = (*ap) & (*bp);
		}
	}
	else { // for negative numbers 2^k modulus
		for (; rp <= stop; rp++, ap++, bp++) {
			*rp = (*ap) ^ (*bp);
		}

		if ((*r->value) < BIG_BASE_M) {
			(*r->value)++;
		}
		else {
			big_t one, t; 
			big_null(&one);
			(*one.value) = 1;
			big_cpy(r, &t);
			big_sum(&t, &one, r);
		}
	}
}

void
big_rst_wrd(big_t *a, int s, big_t *r)
{
	int n = big_get_len(a) - s;
	dig_t *ap = a->value + s, *rp = r->value, *stop = r->value + n;
	big_null(r);
	
	for (; rp <= stop; rp++, ap++) {
		*rp = *ap;
	}

	r->sign = a->sign;
}

void
big_rst(big_t *a, big_t *r)
{
	big_null(r);

	int n = big_get_len(a);
	dig_t *ap = a->value + n, *rp = r->value + n, lsb = 0u;

	for (; rp >= r->value; ap--, rp--) {
		*rp = (uint32_t)(((*ap) >> 1) | (lsb << 31));
		lsb = (*ap) & 1u;
	}

	r->sign = a->sign;
}

void
big_lst_wrd(big_t *a, int s, big_t *r)
{
	int n = big_get_len(a) + s;
	dig_t *ap = a->value, *rp = r->value + s, *stop = r->value + n;
	big_null(r);
	
	for (; rp <= stop; rp++, ap++) {
		*rp = *ap;
	}

	r->sign = a->sign;
}

void
big_lst(big_t *a, big_t *r)
{
	int n = big_get_len(a) + 1;
	twodig_t t, carry = 0;
	dig_t *ap = a->value, *rp = r->value, *stop = r->value + n;
	big_null(r);

	for (; rp <= stop; rp++, ap++) {
		t = ((uint64_t)(*ap) << 1) + carry;
		*rp = (uint32_t)(t & BIG_BASE_M);
		carry = t >> BIG_DIGIT_BITS;
	}

	r->sign = a->sign;
}

void
big_to_hex(big_t *a)
{
	int n = big_get_len(a);
	dig_t *ap = a->value + n;

	fputs("0x", stdout);
	for (; ap >= a->value; --ap) {
		*ap > 0xFFFFFFFu ? printf("%x", *ap) : printf("%08x", *ap);
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
		while (big_gth_uns(r, p) >= 0) {
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
	big_null(r);
	int n = big_get_len(a);
	int m = big_get_len(b);
	int g = n + m + 1, i, j;
	twodig_t uv = 0ull, u = 0ull;
	dig_t *rp = r->value, *ap, *bp = b->value, *tbp;
	
	if (n < m) {
		int t = n;
		n = m;
		m = t;
	}

	for (i = 0; i <= n; i++, rp++) {
		
		ap = a->value;
		bp = b->value + i;

		for (j = 0; j <= i; ap++ ,bp--, j++) {
			uv = uv + ((uint64_t)(*ap) * (uint64_t)(*bp));
			u = (uv >> BIG_DIGIT_BITS) + u;
			uv = uv & BIG_BASE_M;
		}

		*rp = (uint32_t)(uv & BIG_BASE_M);
		uv = u & BIG_BASE_M;
		u = u >> BIG_DIGIT_BITS;
	}

	bp = b->value + n;

	for (; i <= g; i++, rp++) {
		
		ap = a->value + (i - n);

		for (tbp = bp, j = i - n; j <= n; ap++ ,tbp--, j++) {
			uv = uv + ((uint64_t)(*ap) * (uint64_t)(*tbp));
			u = (uv >> BIG_DIGIT_BITS) + u;
			uv = uv & BIG_BASE_M;
		}
		
		*rp = (uint32_t)(uv & BIG_BASE_M);
		uv = u & BIG_BASE_M;
		u = u >> BIG_DIGIT_BITS;
	}

	r->sign = a->sign ^ b->sign;
}

void
big_mod_1305(big_t *a, big_t *p, big_t *r)
{
	big_null(r);

	big_t t1, t2, t3, tmpq, tmpk, pn;
	
	big_null(&tmpk);
	big_cpy(p, &pn);
	big_cpy(a, r);
	r->sign = false;
	*(tmpk.value) = 5u;
	*(pn.value) = BIG_BASE_M;

	while (big_gth_uns(r, p) >= 0) {

		big_sub(r, p, &t3);

		big_rst_wrd(&t3, 4, &t1);
		big_rst(&t1, &tmpq);
		big_rst(&tmpq, &t1);
		big_cpy(&t1, &tmpq);

		big_and(&t3, &pn, &t1);
		big_mul(&tmpq, &tmpk, &t2);
		big_sum(&t1, &t2, r);
		
	}

	if (a->sign == true) {
		big_cpy(r, &t1);
		big_sub(p, &t1, r);
	}
}

void
big_mod_25519(big_t *a, big_t *p, big_t *r)
{
	big_null(r);

	big_t t1, t2, t3, tmpq, tmpk, pn;
	twodig_t msb;
	dig_t *r7 = t3.value + 7;
	
	big_null(&tmpk);
	big_cpy(p, &pn);
	big_cpy(a, r);
	r->sign = false;
	*(tmpk.value) = 19u;
	*(pn.value) = BIG_BASE_M;

	while (big_gth_uns(r, p) >= 0) {
		big_sub(r, p, &t3);
		
		msb = (*r7) & 0x80000000u; // b
		big_rst_wrd(&t3, 8, &t1);
		big_lst(&t1, &tmpq);

		(*tmpq.value) = (*tmpq.value) | (msb >> 31);

		big_and(&t3, &pn, &t1);
		big_mul(&tmpq, &tmpk, &t2);
		big_sum(&t1, &t2, r);
	}
	
	if (a->sign == true) {
		big_cpy(r, &t1);
		big_sub(p, &t1, r);
	}
}

void
big_mod_inv(big_t *a, big_t *b, big_t *r)
{
	big_null(r);

	big_t u, v, x1, x2, one, t; 
	big_cpy(a, &u);
	big_cpy(b, &v);
	big_null(&x1);
	big_null(&x2);
	big_null(&one);
	*(x1.value) = *(one.value) = 1u;

	while (!big_eql(&u, &one) && !big_eql(&v, &one)) {
		while (EVEN(u)) {
			big_rst(&u, &t);
			big_cpy(&t, &u);
			if (!EVEN(x1)) {
				big_sum(&x1, b, &t);
				big_cpy(&t, &x1);
			}
			big_rst(&x1, &t);
			big_cpy(&t, &x1);
		}

		while (EVEN(v)) {
			big_rst(&v, &t);
			big_cpy(&t, &v);
			if (!EVEN(x2)) {
				big_sum(&x2, b, &t);
				big_cpy(&t, &x2);
			}
			big_rst(&x2, &t);
			big_cpy(&t, &x2);
		}

		if (big_gth(&u, &v) >= 0) {
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

void
big_rnd_dig(big_t *r)
{
	big_null(r);
	
	int i;
	dig_t *rp = r->value;
	uint32_t rand_digit;

	for (i = 0; i < 8; i++) {
		_rdseed32_step(&rand_digit);
		*(rp++) = rand_digit;
	}

}

void
big_sum_25519(big_t *a, big_t *b, big_t *p, big_t *r)
{
	big_t t1;
	big_sum(a, b, &t1);
	big_mod_25519(&t1, p, r);
}

void
big_sub_25519(big_t *a, big_t *b, big_t *p, big_t *r)
{
	big_t t1;	
	big_sub(a, b, &t1);
	big_mod_25519(&t1, p, r);
}

void
big_mul_25519(big_t *a, big_t *b, big_t *p, big_t *r)
{
	big_t t1;	
	big_mul(a, b, &t1);
	big_mod_25519(&t1, p, r);
}