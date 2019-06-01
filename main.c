#include "newbignum.h"
#include <time.h>

static char P25519[] 				= "7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed";
static char BK_MINUS_25519[] 		= "100000000000000000000000000000000000000000000000000000000";
static char BK_PLUS_25519[] 		= "1000000000000000000000000000000000000000000000000000000000000000000000000";
static char BK_PLUS_25519_MINUS[] 	= "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
static char U_25519[] 				= "20000000000000000000000000000000000000000000000000000000000000000";
static char BETA_25519[] 			= "d0d79435e50d79435e50d79435e50d79435e50d79435e50d79435e50d79435e5";
static char R_25519[] 				= "10000000000000000000000000000000000000000000000000000000000000000";
static char R_MINUS_25519[] 		= "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
static char PN_25519[] 				= "7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
static char A_25519[] 				= "26";

int
main(int argc, char const *argv[])
{
    clock_t start, end;
    double cpu_time_used;
	big_t *a = big_new();
	big_t *b = big_new();
	big_t *c = big_new();
	big_t *d = big_new();
	big_t *p = big_new();
	big_t *r = big_new();
	big_t *u = big_new();
	big_t *bk_minus = big_new();
	big_t *bk_plus = big_new();
	big_t *bk_plus_minus = big_new();
	big_t *A = big_new();
	big_t *R = big_new();
	big_t *beta = big_new();
	big_t *Rm = big_new();
	big_t *pn = big_new();

	hex_to_big(U_25519, u);
	hex_to_big(BETA_25519, beta);
	beta->sign = true;
	hex_to_big(R_25519, R);
	hex_to_big(A_25519, A);
	hex_to_big(R_MINUS_25519, Rm);
	hex_to_big(BK_PLUS_25519_MINUS, bk_plus_minus);
	hex_to_big(BK_PLUS_25519, bk_plus);
	hex_to_big(BK_MINUS_25519, bk_minus);
	hex_to_big(P25519, p);
	hex_to_big(U_25519, u);
	hex_to_big("35fedf799f98ffaefb6fb91d77db7dc8fc8ff23fb5dc8fd77db7dc8ff3fc23f9", a);
	hex_to_big("3ace9e4bddc3029198a2be2ef84826ea23060628308a93ec90170e02654f33df", b);
	hex_to_big("973a9", c);
	hex_to_big("eb3a792f770c0a46628af8bbe1209ba88c1818a0c22a4fb2405d292a1c064aa2654f33df", d);
	hex_to_big(PN_25519, pn);

	printf("Mult\n");
	start = clock();
    big_mul(a, b, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Sub\n");
	start = clock();
    big_sub(b, a, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Sum\n");
	start = clock();
    big_sum(a, b, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Power Mod\n");
	start = clock();
	big_mont_pow(a, c, p, A, R, beta, Rm, pn, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Mod Inverse\n");
	start = clock();
    big_mod_inv(a, p, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Mod\n");
	start = clock();
	big_fastmod_25519(d, p, pn, r);
	//big_barrett_mod(d, p, u, bk_minus, bk_plus, bk_plus_minus, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("One Rshift\n");
	start = clock();
    big_rst(a, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("32 rshift\n");
	start = clock();
    big_rst_word(a, 1, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("And Gate\n");
	start = clock();
    big_and(a, b, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("L shift\n");
	start = clock();
    big_lst(a, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("32 lshift\n");
	start = clock();
    big_lst_word(a, 1, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");
	
	hex_to_big(A_25519, A);
	printf("legendre\n");
	start = clock();
	int x = big_legendre_symbol(a, p, A, R, beta, Rm, pn);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("%d\n", x);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("square\n");
	start = clock();
    big_sqr(a, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	big_free(a);
	big_free(b);
	big_free(c);
	big_free(d);
	big_free(p);
	big_free(r);
	big_free(u);
	big_free(bk_plus);
	big_free(bk_minus);
	big_free(bk_plus_minus);
	big_free(A);
	big_free(R);
	big_free(Rm);
	big_free(beta);
	big_free(pn);
	return 0;
}