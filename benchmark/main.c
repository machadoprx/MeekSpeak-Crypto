#include "../bn/bn.h"
#include "../ecc/ecc_25519.h"
#include <time.h>

#define f_apply(type, f, ...) {									\
	void *stop_loop = (int[]){0};								\
	type **list = (type*[]){__VA_ARGS__, stop_loop};			\
	for (int i = 0; list[i] != stop_loop; i++)					\
		f(list[i]);												\
}

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

	big_t *a, *b, *c, *d, *p, *r, *A, *R, *beta, *Rm, *l, *np;
	big_new(a);	big_new(b);
	big_new(c);	big_new(d);
	big_new(p);	big_new(r);
	big_new(A);	big_new(R);
	big_new(beta); big_new(Rm);
	big_new(l);	big_new(np);

	hex_to_big(BETA_25519, beta);
	hex_to_big(R_25519, R);
	hex_to_big(A_25519, A);
	hex_to_big(R_MINUS_25519, Rm);
	hex_to_big(P25519, p);
	hex_to_big(N_25519, np);
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
    big_and(beta, Rm, r);
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
	big_null(r);

	printf("rand 8 digits\n");
	start = clock();
	big_to_hex(np);
	big_rand_8dig(r);
	while (big_gth_uns(r, np)) {
    	big_rand_8dig(r);
	}
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	f_apply(void, free, PR, PR2, PR3, curvetest->G, curvetest, a, b, c, d, l, p, r, A, R, Rm, beta);
	
	return 0;
}