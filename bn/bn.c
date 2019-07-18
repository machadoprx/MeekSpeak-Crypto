#include "bn.h"

int
big_get_len(big_t *a)
{
	register int k = BIG_MAX_DIGITS - 1;
	register dig_t *ap = a->value + k;

	while (*ap == 0ull && --k > 0) {
		ap--;
	}

	return k;
}

dig_t*
big_to_bin(big_t *a, int *lenght)
{
	dig_t *bin = malloc(512 * sizeof(dig_t));
	register dig_t *p = bin;
	register int k = big_get_len(a);	
	register dig_t *ap = a->value + k, t = *(ap), mask = 0x80000000ull;
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
		while (mask != 0ull) {
			*p++ = (mask & (*ap));
			mask = mask >> 1;
		}
		mask = 0x80000000ull;
	}

	*lenght = mswbits + (k * BIG_DIGIT_BITS);

	return bin;
}

dig_t
bin_to_int(char *num)
{
	register dig_t r = 0;
	register char *p = num, *stop = num + BIG_DIGIT_BITS - 1;

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
	return a->sign ? 0 : 2;
}

int 
big_gth_uns(big_t *a, big_t *b)
{
	register int n = big_get_len(a);
	register int m = big_get_len(b);
	
	if (n != m) {
		return n > m ? 2 : 0;
	}

	register dig_t *ap = a->value + n;
    register dig_t *bp = b->value + n;
	register int i = n;

	while (i > 0 && (*ap == *bp)) {
		ap--; bp--; i--;
	}

	return (*ap < *bp) ? 0 : (*ap == *bp) ? 1 : 2;
}

static void
padd_str(char *str, char *dest, int padd)
{
	register char *p = str + (strlen(str) - 1);
	register char *m = dest + padd - 1;
	memset(dest, '0', sizeof(char) * padd);

	while (*p) {
		*m-- = *p--;
	}
}

void
bin_to_big(char *src, big_t *r)
{
	big_null(r);

	char num[512], digit[BIG_DIGIT_BITS];
	padd_str(src, num, 512);

	register char *nump = num;
	register dig_t *rp = r->value + (BIG_MAX_DIGITS - 1);
	register char *digp, *stop = digit + BIG_DIGIT_BITS - 1;

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

	register char *nump = num;
	register dig_t *rp = r->value + (BIG_MAX_DIGITS - 1);
	register char *digp, *stop = digit + 7;

	for (; rp >= r->value; rp--) {
		digp = digit;

		while (digp <= stop) {
			*(digp++) = *(nump++);
		}
		*(digp) = '\0';

		*rp = strtoull(digit, NULL, 16);
	}
}

int
big_legendre_symbol(big_t *a, big_t *b, big_t *A, big_t *R, big_t *beta, big_t *Rm)
{
	big_t t, r;
	big_rst(b, &t);
	big_mont_pow(a, &t, b, A, R, beta, Rm, &r);
	return ((*r.value) > 1ull) ? -1 : (*r.value);
}

void
big_mont(big_t *a, big_t *b, big_t *Rm, big_t *beta, big_t *r)
{
	big_null(r);

	big_t t1;
	big_and(a, Rm, &t1);
	big_mul(&t1, beta, r);
	big_and(r, Rm, &t1);
	big_mul(&t1, b, r);
	big_sum(a, r, &t1);
	big_rst_word(&t1, 8, r);

	if (big_gth_uns(r, b) > BIG_LESS) {
		big_sub(r, b, &t1);
		big_cpy(&t1, r);
	}
}

void
big_mont_pow(big_t *a, big_t *b, big_t *p, big_t *A, big_t *R, big_t *beta, big_t *Rm, big_t *r)
{
	big_null(r);

	big_t xn, t;
	int b_lenght;
	dig_t *bin_b = big_to_bin(b, &b_lenght);
	register dig_t *bit = bin_b;
	big_mul(a, R, &t);
	big_fastmod_25519(&t, p, &xn); // for p = p25519 only

	while (b_lenght-- > 0) {
		big_mul(A, A, &t);
		big_mont(&t, p, Rm, beta, A);
		if (*(bit++) != 0) {
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
	register dig_t w, carry = 0;
	register dig_t *gp, *lp, *rp = r->value;

	if (n >= m) {
		lp = b->value;
		gp = a->value;
	}
	else {
		lp = a->value;
		gp = b->value;
		int t = n; n = m; m = t;
	}
	
	register dig_t *stop1 = r->value + n + 1, *stop2 = r->value + m;

	for (; rp <= stop2; lp++, gp++, rp++) {
		w = carry + (*gp) + (*lp);
		*rp = w & BIG_BASE_M;
		carry = w >> BIG_DIGIT_BITS;
	}

	while (rp <= stop1) {
		w = carry + *(gp++);
		*(rp++) = w & BIG_BASE_M;
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
	register dig_t *gp, *lp, *rp = r->value, borrow = 0;
	big_null(r);

	if (big_gth_uns(a, b) > BIG_LESS) {
		gp = a->value;
		lp = b->value;
	}
	else {
		gp = b->value;
		lp = a->value;
		t = n; n = m; m = t; m++;
	}

	register dig_t *stop1 = r->value + n, *stop2 = r->value + m, shift = ((8 * sizeof(dig_t)) - 1);

	for (; rp <= stop2; gp++, lp++, rp++) {

		*rp = (*gp) - (*lp) - borrow;
		borrow = (*rp) >> shift;
		*rp = (*rp) & BIG_BASE_M;
	}

	while (rp <= stop1) {

		*rp = (*gp) - borrow;
		borrow = (*rp) >> shift;
		*rp = (*rp) & BIG_BASE_M;

		rp++; gp++;
	}

	r->sign = (t == -1) ? false : true;
}

void
big_sqr(big_t *a, big_t *r)
{
	big_null(r);
	int n = big_get_len(a);
	register twodig_t uv;
	register dig_t *tmpt, *ap = a->value, *rp = r->value, *tmpap, tmpx, u, *stop = (a->value + n);

	for (; ap <= stop; ap++, rp += 2) {
		
		uv = (*rp) + ((*ap) * (*ap));
		(*rp) = uv & BIG_BASE_M;
		u = uv >> BIG_DIGIT_BITS;
		tmpx = (*ap);
		tmpap = ap + 1;
		tmpt = rp + 1;

		for (; tmpap <= stop; tmpap++, tmpt++) {
			uv = tmpx * (*tmpap);
			uv = (*tmpt) + (uv << 1) + u;
			*tmpt = uv & BIG_BASE_M;
			u = uv >> BIG_DIGIT_BITS;
		} 

		while (u != 0ull) {
			uv = (*tmpt) + u;
			*(tmpt++) = uv & BIG_BASE_M;
			u = uv >> BIG_DIGIT_BITS;
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

	int n = big_get_len(a);
	int m = big_get_len(b);
	int g = n >= m ? n : m;
	register dig_t *ap = a->value, *bp = b->value, *stop = a->value + g;

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
	register dig_t *ap = a->value, *bp = b->value, *rp = r->value, *stop = r->value + g;

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
big_rst_word(big_t *a, int s, big_t *r)
{
	int n = big_get_len(a) - s;
	register dig_t *ap = a->value + s, *rp = r->value, *stop = r->value + n;
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
	register dig_t *ap = a->value + n, *rp = r->value + n, lsb = 0ull;

	for (; rp >= r->value; ap--, rp--) {

		*rp = ((*ap) >> 1) | (lsb << 31);
		lsb = (*ap) & 1ull;
	}

	r->sign = a->sign;
}

void
big_lst_word(big_t *a, int s, big_t *r)
{
	int n = big_get_len(a) + s;
	register dig_t *ap = a->value, *rp = r->value + s, *stop = r->value + n;
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
	register dig_t t, carry = 0;
	register dig_t *ap = a->value, *rp = r->value, *stop = r->value + n;
	big_null(r);

	for (; rp <= stop; rp++, ap++) {
		t = (*ap << 1ull) + carry;
		*rp = t & BIG_BASE_M;
		carry = t >> BIG_DIGIT_BITS;
	}

	r->sign = a->sign;
}

void
big_to_hex(big_t *a)
{
	int n = big_get_len(a);
	register dig_t *ap = a->value + n;

	fputs("0x", stdout);
	for (; ap >= a->value; --ap) {
		*ap > 0xFFFFFFFull ? printf("%lx", *ap) : printf("%08lx", *ap);
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
	big_null(r);
	register int n = big_get_len(a);
	register int m = big_get_len(b);
	register int g = n + m + 1, i, j;
	register dig_t uv = 0ull, u = 0ull, *rp = r->value, *ap, *bp, *tbp;
	
	if (n < m) {
		int t = n;
		n = m; m = t;
	}

	bp = b->value;
	for (i = 0; i <= n; i++, rp++) {
		
		ap = a->value;
		bp = b->value + i;

		for (j = 0; j <= i; ap++ ,bp--, j++) {
			uv = uv + ((*ap) * (*bp));
			u = (uv >> BIG_DIGIT_BITS) + u;
			uv = uv & BIG_BASE_M;
		}

		(*rp) = uv & BIG_BASE_M;
		uv = u & BIG_BASE_M;
		u = u >> BIG_DIGIT_BITS;
	}

	bp = b->value + n;

	for (; i <= g; i++, rp++) {
		
		ap = a->value + (i - n);

		for (tbp = bp, j = i - n; j <= n; ap++ ,tbp--, j++) {
			uv = uv + ((*ap) * (*tbp));
			u = (uv >> BIG_DIGIT_BITS) + u;
			uv = uv & BIG_BASE_M;
		}
		
		(*rp) = uv & BIG_BASE_M;
		uv = u & BIG_BASE_M;
		u = u >> BIG_DIGIT_BITS;
	}

	r->sign = a->sign ^ b->sign;
}

void
big_fastmod_25519(big_t *a, big_t *p, big_t *r)
{
	big_null(r);

	big_t t1, t2, t3, tmpq, tmpk, pn;
	register dig_t msb, *r7 = r->value + 7;
	
	big_null(&tmpk);
	big_cpy(p, &pn);
	big_cpy(a, r);
	r->sign = false;
	*(tmpk.value) = K_25519;
	*(pn.value) = BIG_BASE_M;

	while (big_gth_uns(r, p) > BIG_LESS) {

		msb = (*r7) & 0x80000000ull; // b
		big_rst_word(r, 8, &t1);
		big_lst(&t1, &tmpq);

		(*tmpq.value) = (*tmpq.value) | (msb >> 31);

		big_and(r, &pn, &t1);
		big_mul(&tmpq, &tmpk, &t2);
		big_sum(&t1, &t2, r);
		
		if (big_gth_uns(r, p) > BIG_LESS) {
			
			big_sub(r, p, &t3);
			big_cpy(&t3, r);
		}
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
	*(x1.value) = *(one.value) = 1ull;

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

		if (big_gth(&u, &v) > BIG_LESS) {
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
big_rand_8dig(big_t *r)
{
	big_null(r);
	
	int i;
	register dig_t *rp = r->value;
	uint32_t key[8], nounce[3], out[1][16];
	unsigned int rand_digit;

	for (i = 0; i < 8; i++) {
		_rdseed32_step(&rand_digit);
		key[i] = rand_digit;
	}

	for (i = 0; i < 3; i++) {
		_rdseed32_step(&rand_digit);
		nounce[i] = rand_digit;
	}

	chacha_enc(key, nounce, 0, 1, 8, out);	

	for (i = 0; i < 8; i++) {
		*(rp++) = (*out)[i] ^ (*out)[15 - i];
	}

}

void
big_mul_kat(big_t *a, big_t *b, big_t *r)
{
	big_null(r);

	big_t x0, x1, y0, y1, t1, x0y0, x1y1, betaB, betaBmod, tmp;
	bool asign = a->sign, bsign = b->sign;
	a->sign = b->sign = false;
	int n = big_get_len(a), m = big_get_len(b);
	int g = (n > m) ? m : n;
	g = g >> 1;

	big_null(&betaB);
	big_null(&betaBmod);
	betaB.value[g] = 1ull;
	memset(betaBmod.value, BIG_BASE_M, g * sizeof(dig_t));

	big_and(a, &betaBmod, &x0);
	big_and(b, &betaBmod, &y0);
	big_rst_word(a, g, &x1);
	big_rst_word(b, g, &y1);
	// (2^32)^g == colocar 1 na posição g
	big_mul(&x0, &y0, &x0y0);
	big_mul(&x1, &y1, &x1y1);
	big_sum(&x1, &x0, &t1);
	big_sum(&y1, &y0, &x0);
	big_cpy(&t1, &tmp);
	big_mul(&x0, &tmp, &t1);

	big_sum(&x0y0, &x1y1, &x0);
	big_sub(&t1, &x0, &tmp);

	big_lst_word(&tmp, g, &t1);
	big_cpy(&x1y1, &tmp);
	big_lst_word(&tmp, 2 * g, &x1y1);
	big_cpy(&t1, &tmp);
	big_sum(&tmp, &x0y0, &t1);

	big_sum(&t1, &x1y1, r);
	r->sign = asign ^ bsign;
	a->sign = asign;
	b->sign = bsign;
}