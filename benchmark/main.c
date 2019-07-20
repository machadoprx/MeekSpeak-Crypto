#include "../bn/bn.h"
#include "../ecc/ecc_25519.h"
#include <time.h>

#define f_apply(type, f, ...) {									\
	void *stop_loop = (int[]){0};								\
	type **list = (type*[]){__VA_ARGS__, stop_loop};			\
	for (int i = 0; list[i] != stop_loop; i++)					\
		f(list[i]);												\
}

int
main(int argc, char const *argv[])
{
	clock_t start, end;
    double cpu_time_used;

	big_t *a, *b, *c, *d, *p, *r, *l, *np;
	big_new(a);	big_new(b);
	big_new(c);	big_new(d);
	big_new(p);	big_new(r);
	big_new(l);	big_new(np);

	hex_to_big("35fedf799f98ffaefb6fb91d77db7dc8fc8ff23fb5dc8fd77db7dc8ff3fc23f9", a);
	hex_to_big("3ace9e4bddc3029198a2be2ef84826ea23060628308a93ec90170e02654f33df", b);
	hex_to_big("1790f520c6645bdc6192b7da46c9382a5b9d8bf3e856a96e2c7018bc46f38534", l);
	hex_to_big("9ac6241f", c);
	hex_to_big("30591451fdebaf7c7c0457f47a3139c5db1bde9faa002f53134d7bb030ed3bbcebcd28b466227cc87766421df596a50c58c21d04c88ebf9ed887b58bf7112dc", d);
	
	memcpy(p->value, P25519, sizeof(dig_t) * 8);
	memcpy(np->value, N25519, sizeof(dig_t) * 8);

	printf("Mod\n");
	start = clock();
	big_mod_25519(d, p, r);
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
	big_mnt_pow_25519(a, c, r);
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
    big_rst_wrd(a, 1, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("And Gate\n");
	start = clock();
    big_and(d, a, r);
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
    big_lst_wrd(a, 1, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("legendre\n");
	start = clock();
	int x = big_lgd_sym(a, p);
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
	ecp_t *PR, *PR2, *PR3;
	ecp_new(PR);
	ecp_new(PR2);
	ecp_new(PR3);

	printf("curve double\n");
	start = clock();
	ecp_dbl(curvetest, curvetest->G, p, PR);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	ecp_get_afn(PR, p, r);
	big_to_hex(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("curve add\n");
	start = clock();
	ecp_add(curvetest, PR, curvetest->G, p, PR2);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	ecp_get_afn(PR2, p, r);
	big_to_hex(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("curve mult\n");
	start = clock();
	ecp_mul(curvetest, curvetest->G, l, p, PR3);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	ecp_get_afn(PR3, p, r);
	big_to_hex(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");
	big_null(r);

	printf("rand 8 digits\n");
	start = clock();
	big_to_hex(np);
	big_rnd_dig(r);
	r->value[7] &= 0x7FFFFFFF;
	while (big_gth_uns(r, np)) {
    	big_rnd_dig(r);
		r->value[7] &= 0x7FFFFFFF;
	}
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	f_apply(void, free, PR, PR2, PR3, curvetest->G, curvetest, a, b, c, d, l, p, r);
	
	return 0;
}