#include "newbignum.h"

big_t*
big_new()
{
	big_t *t = malloc(sizeof(struct _big_t));
	t->value = malloc(WORD_LENGHT * DIGIT_SIZE * DIGIT_SIZE);
	memset(t->value, 0, WORD_LENGHT * DIGIT_SIZE * DIGIT_SIZE);
	t->sign = false;
	return t;
}

void
big_free(big_t *a)
{
	if(a == NULL){
		return;
	}
	free(a->value);
	free(a);
}

char*
big_to_bin(const big_t *a)
{
	char *bin = malloc(DIGIT_SIZE * DIGIT_SIZE);
	char *p = bin;
	int k = DIGIT_SIZE - 1;
	memset(bin, 0, DIGIT_SIZE * DIGIT_SIZE);
	while(k >= 0 && a->value[k--] == 0);
	for(int i = k + 1; i >= 0; --i){	
		for(uint64_t mask = 0x80000000u; mask > 0; mask >>= 1){
			*p++ = (mask & a->value[i]) ? '1' : '0';
		}
	}
	return bin;
}

void
big_null(big_t *a)
{
	memset(a->value, 0, WORD_LENGHT * DIGIT_SIZE * DIGIT_SIZE);
	a->sign = false;
}

static uint64_t
bin_to_int(const char *num)
{
	uint64_t r = 0;
	for(int i = 0; i < DIGIT_SIZE; ++i){
		r = r << 1;
		if(num[i] == '1'){
			r = r | 1;
		}
	}
	return r;
}

int 
big_gth(const big_t *a, const big_t *b)
{
	if(a->value == NULL || b->value == NULL){
		return 0;
	}
	if(a->sign == false && b->sign == false){
		for(int i = DIGIT_SIZE - 1; i >= 0; i--){
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
		for(int i = DIGIT_SIZE - 1; i >= 0; i--){
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

int 
big_gth_uns(const big_t *a, const big_t *b)
{
	if(a->value == NULL || b->value == NULL){
		return 0;
	}
	for(int i = DIGIT_SIZE - 1; i >= 0; i--){
		if(a->value[i] < b->value[i]){
			return 0;
		}
		else if(a->value[i] > b->value[i]){
			return 2;
		}
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
	if(src == NULL || r == NULL){
		return;
	}

	char *num = padd_str(src);
	big_null(r);
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

int
big_legendre_symbol(const big_t *a, const big_t *b)
{
	if(a == NULL || b == NULL){
		return -2;
	}

	big_t *t = big_new();
	big_t *r = big_new();
	uint64_t x;

	big_cpy(b, t);
	t->value[0] -= 1;
	big_rst(t, r);
	big_mont_pow_25516(a, r, t);
	big_free(r);
	x = t->value[0];
	big_free(t);
	if(x > 1){
		return -1;
	}
	return (int) x;
}

static void
big_mont_25516(const big_t *a, const big_t *b, const big_t *Rm, const big_t *beta, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}

	big_t *t1 = big_new();
	big_null(r);
	big_and(a, Rm, t1);
	big_mul(t1, beta, r);
	big_and(r, Rm, t1);
	big_mul(t1, b, r);
	big_sum(a, r, t1);
	big_rst_word(t1, 8, r);
	if(big_gth(r, b) >= EQUAL){
		big_sub(r, b, t1);
		big_cpy(t1, r);
	}
	big_free(t1);
}

void
big_mont_pow_25516(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}

	big_t *xn = big_new();
	big_t *A = big_new();
	big_t *R = big_new();
	big_t *p = big_new();
	big_t *beta = big_new();
	big_t *Rm = big_new();
	big_t *t = big_new();
	char *bin_b = big_to_bin(b);
	int k = 1;
	
	bin_to_big(R_MINUS_25519, Rm);
	bin_to_big(P25519, p);
	bin_to_big(BETA_25519, beta);
	beta->sign = true;
	bin_to_big(R_25519, R);
	bin_to_big(A_25519, A);
	big_mul(a, R, t);
	big_fst_25519_mod(t, xn);
	while(k < strlen(bin_b) && bin_b[k] == '0'){
		k++;
	}
	for(int i = k; i < strlen(bin_b); ++i){
		big_mul(A, A, t);
		big_mont_25516(t, p, Rm, beta, A);
		if(bin_b[i] == '1'){
			big_mul(A, xn, t);
			big_mont_25516(t, p, Rm, beta, A);
		}
	}
	big_mont_25516(A, p, Rm, beta, r);
	big_free(xn); big_free(A);
	big_free(p); big_free(Rm);
	big_free(beta); big_free(t);
	big_free(R); free(bin_b);
}

void
big_sum(const big_t *a, const big_t *b, big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}
	if(a->sign != b->sign){
		big_t *a1 = big_new();
		big_cpy(a, a1);
		a1->sign = false;
		big_t *b1 = big_new();
		big_cpy(b, b1);
		b1->sign = false;
		if(a->sign == false){
			big_sub(a1, b1, r);
		}
		else{
			big_sub(b1, a1, r);
		}
		big_free(a1);
		big_free(b1);
		return;
	}
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
		big_t *a1 = big_new();
		big_cpy(a, a1);
		a1->sign = false;
		big_t *b1 = big_new();
		big_cpy(b, b1);
		b1->sign = false;
		big_sum(a1, b1, r);
		if(a->sign == true){
			r->sign = true;
		}
		big_free(a1);
		big_free(b1);
		return;
	}
	if(a->sign == true){
		big_t *a1 = big_new();
		big_cpy(a, a1);
		a1->sign = false;
		big_t *b1 = big_new();
		big_cpy(b, b1);
		b1->sign = false;
		big_sub(b1, a1, r);
		big_free(a1);
		big_free(b1);
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
	r->value[0] = w % BASE;
	for(int i = 1; i < DIGIT_SIZE * 32 * 32; ++i){
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
		r->value[i] = w % BASE;
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
	for(int i = 0; i < DIGIT_SIZE; ++i){
		hi = 0;
		for(int j = 0; j < DIGIT_SIZE; ++j){
			temp = r->value[i + j] + (a->value[i] * b->value[j]) + hi;
			hi = (temp >> 32) & 0xFFFFFFFF;
			lo = temp & 0xFFFFFFFF;
			r->value[i + j] = lo;
		} 
		r->value[i + DIGIT_SIZE - 1] = hi;
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

	for(int i = DIGIT_SIZE - 1; i >= 0; i--){
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
	if(a->sign == false){
		for(int i = 0; i < DIGIT_SIZE; i++){
			r->value[i] = a->value[i] & b->value[i];
		}
	}
	else{
		int k = DIGIT_SIZE - 1;
		while (k >= 0 && (a->value[k] == 0 || b->value[k] == 0)){
			k--;
		}
		for(int i = 0; i <= k; i++){
			r->value[i] = (a->value[i] ^ b->value[i]);
		}
		if(r->value[0] < (BASE - 1)){
			r->value[0]++;
		}
		else{
			big_t *t = big_new();
			big_t *one = big_new();
			bin_to_big("1", one);
			big_sum(one, r, t);
			big_cpy(t, r);
			big_free(t);
			big_free(one);
		}
	}
}

void
big_lst_word(const big_t *a, uint32_t n, big_t *r)
{
	if(a == NULL || r == NULL || n == 0){
		return;
	}
	big_t *z = big_new();
	bin_to_big("0", z);
	big_null(r);
	if(big_eql(z, a)){
		big_free(z);
		big_cpy(a, r);
		return;
	}
	big_free(z);
	for(int i = 0; i < DIGIT_SIZE; i++){
		r->value[i + n] = a->value[i];
	}
	r->sign = a->sign;
}

void
big_rst_word(const big_t *a, uint32_t n, big_t *r)
{
	if(a == NULL || r == NULL || n == 0){
		return;
	}
	
	big_t *z = big_new();
	bin_to_big("0", z);
	big_null(r);
	if(big_eql(z, a)){
		big_free(z);
		big_cpy(a, r);
		return;
	}
	big_null(r);
	for(int i = 0; i < DIGIT_SIZE; i++){
		r->value[i] = a->value[i + n];
	}
	big_free(z);
	r->sign = a->sign;
}

void
big_rst(const big_t *a, big_t *r)
{
	if(a == NULL || r == NULL){
		return;
	}

	big_null(r);
	int lsb = 0;
	for(int i = DIGIT_SIZE - 1; i >= 0; --i){
		r->value[i] = a->value[i] >> 1;
		if(lsb == 1){
			r->value[i] = (a->value[i] | 0x100000000) >> 1;
		}
		lsb = a->value[i] & 1;
	}
	r->sign = a->sign;
}

void
big_cpy(const big_t *a, big_t *r)
{
	if(a == NULL || r == NULL){
		return;
	}

	memcpy(r->value, a->value, WORD_LENGHT * DIGIT_SIZE * DIGIT_SIZE);
	r->sign = a->sign; 
}

void
big_to_hex(const big_t *a)
{
	int k = DIGIT_SIZE -1;
	while(a->value[k] == 0 && k >= 0){
		k--;
	}
	fputs("0x", stdout);
	for(int i = k; i >= 0; --i){
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
	if(r->sign == false){
		while(big_gth_uns(r, p) > 0){
			big_t *tmp = big_new();
			big_sub(r, p, tmp);
			big_cpy(tmp, r);
			big_free(tmp);
		}
	}
	else{
		while(r->sign == true){
			big_t *tmp = big_new();
			big_sum(r, p, tmp);
			big_cpy(tmp, r);
			big_free(tmp);
		}
	}
}

void
big_fst_25519_mod(const big_t *a, big_t *r)
{
	if(a == NULL || r == NULL){
		return;
	}
	big_null(r);

	big_t *p = big_new();
	bin_to_big(P25519, p);
	big_t *u = big_new();
	bin_to_big(U_25519, u);
	big_t *bk_minus = big_new();
	bin_to_big(BK_MINUS_25519, bk_minus);
	big_t *bk_plus = big_new();
	bin_to_big(BK_PLUS_25519, bk_plus);
	big_t *bk_plus_minus = big_new();
	bin_to_big(BK_PLUS_25519_MINUS, bk_plus_minus);
	big_t *t1 = big_new();
	big_t *t2 = big_new();
	big_t *t3 = big_new();

	big_cpy(a, t2);
	big_rst_word(t2, 7, t1);
	big_cpy(t1, t2);
	big_mul(u, t1, t2);
	big_cpy(t2, t1);
	big_rst_word(t1, 9, t2);
	big_cpy(t2, t1);

	big_and(a, bk_plus_minus, t1);
	big_mul(p, t2, t3); 
	big_and(t3, bk_plus_minus, t2);
	big_sub(t1, t2, r);

	bin_to_big("0", t3);
	if(r->sign == true){
		big_sum(r, bk_plus, t1);
		big_cpy(t1, r);
	}
	while(big_gth_uns(r, p) >= EQUAL){
		big_sub(r, p, t1);
		big_cpy(t1, r);
	}
	big_free(p); big_free(u);
	big_free(bk_minus); big_free(bk_plus);
	big_free(bk_plus_minus); big_free(t1);
	big_free(t2); big_free(t3);
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
big_mod_inv(const big_t *a, const big_t *b, void(*mod)(const big_t*, big_t*), big_t *r)
{
	if(a == NULL || b == NULL || r == NULL){
		return;
	}

	big_null(r);
	if(big_gth(a, b)){
		return;
	}
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

	while(!big_eql(u, one) && !big_eql(v, one)){
		while(!big_odd(u)){
			big_t *tmp = big_new();
			big_rst(u, tmp);
			big_cpy(tmp, u);
			if(big_odd(x1)){
				big_sum(x1, b, tmp);
				big_cpy(tmp, x1);
			}
			big_rst(x1, tmp);
			big_cpy(tmp, x1);
			big_free(tmp);
		}
		while(!big_odd(v)){
			big_t *tmp = big_new(); 
			big_rst(v, tmp);
			big_cpy(tmp, v);
			if(big_odd(x2)){
				big_sum(x2, b, tmp);
				big_cpy(tmp, x2);
			}
			big_rst(x2, tmp);
			big_cpy(tmp, x2);
			big_free(tmp);
		}
		if(big_gth(u, v) >= EQUAL){
			big_t *tmp = big_new();
			big_sub(u, v, tmp);
			big_cpy(tmp, u);
			big_sub(x1, x2, tmp);
			big_cpy(tmp, x1);
			big_free(tmp);
		}
		else{
			big_t *tmp = big_new();
			big_sub(v, u, tmp);
			big_cpy(tmp, v);
			big_sub(x2, x1, tmp);
			big_cpy(tmp, x2);
			big_free(tmp);
		}
	}
	if(big_eql(u, one)){
		(*mod)(x1, r);
	}
	else{
		(*mod)(x2, r);
	}
	big_free(x1);
	big_free(x2);
	big_free(one);
	big_free(u);
	big_free(v);
}