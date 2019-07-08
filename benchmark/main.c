#include "../bn/bn.h"
#include "../ecc/ecc_25519.h"
#include <time.h>

static char P25519[] 		= "7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed";
static char BETA_25519[] 	= "d0d79435e50d79435e50d79435e50d79435e50d79435e50d79435e50d79435e5";
static char R_25519[] 		= "10000000000000000000000000000000000000000000000000000000000000000";
static char R_MINUS_25519[] = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
static char A_25519[] 		= "26";
static char N_25519[]		= "1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed";

int
main(int argc, char const *argv[])
{
	clock_t start, end;
    double cpu_time_used;

	big_t *a = big_new(); big_t *b = big_new();
	big_t *c = big_new(); big_t *d = big_new();
	big_t *p = big_new(); big_t *r = big_new();
	big_t *A = big_new(); big_t *R = big_new();
	big_t *beta = big_new(); big_t *Rm = big_new();
	big_t *l = big_new();

	hex_to_big(BETA_25519, beta);
	hex_to_big(R_25519, R);
	hex_to_big(A_25519, A);
	hex_to_big(R_MINUS_25519, Rm);
	hex_to_big(P25519, p);
	hex_to_big("35fedf799f98ffaefb6fb91d77db7dc8fc8ff23fb5dc8fd77db7dc8ff3fc23f9", a);
	hex_to_big("3ace9e4bddc3029198a2be2ef84826ea23060628308a93ec90170e02654f33df", b);
	hex_to_big("1790f520c6645bdc6192b7da46c9382a5b9d8bf3e856a96e2c7018bc46f38534", l);
	hex_to_big("9ac6241f", c);
	hex_to_big("30591451fdebaf7c7c0457f47a3139c5db1bde9faa002f53134d7bb030ed3bbcebcd28b466227cc87766421df596a50c58c21d04c88ebf9ed887b58bf7112dc", d);
	beta->sign = true;

	printf("Mod\n");
	start = clock();
	big_fastmod_25519(d, p, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

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
	big_mont_pow(a, c, p, A, R, beta, Rm, r);
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
	int x = big_legendre_symbol(a, p, A, R, beta, Rm);
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

	ec_t *curvetest = ec_init_c25519();
	ecp_t *PR = ecp_new();
	ecp_t *PR2 = ecp_new();
	ecp_t *PR3 = ecp_new();

	printf("curve double\n");
	start = clock();
	ecp_double(curvetest, curvetest->G, p, PR);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	ecp_get_affine(PR, p, r);
	big_to_hex(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("curve add\n");
	start = clock();
	ecp_add(curvetest, PR, curvetest->G, p, PR2);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	ecp_get_affine(PR2, p, r);
	big_to_hex(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("curve mult\n");
	start = clock();
	ecp_mul(curvetest, curvetest->G, l, p, PR3);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	ecp_get_affine(PR3, p, r);
	big_to_hex(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("rand 8 digits\n");
	start = clock();
    big_rand_8dig(r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	free(PR); free(PR2);
	free(PR3); free(curvetest->G);
	free(curvetest); big_free(a);
	big_free(b); big_free(c);
	big_free(d); big_free(l);
	big_free(p); big_free(r);
	big_free(A); big_free(R);
	big_free(Rm); big_free(beta);
	
	return 0;
}