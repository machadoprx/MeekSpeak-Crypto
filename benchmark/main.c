#include "../bn/bn.h"
#include "../ecc/ecc_25519.h"
#include <time.h>

int
main(int argc, char const *argv[])
{
	clock_t start, end;
    double cpu_time_used;

	big_t a, b, c, d, p, r, l, np;
	big_null(&a);	big_null(&b);
	big_null(&c);	big_null(&d);
	big_null(&p);	big_null(&r);
	big_null(&l);	big_null(&np);

	hex_to_big("35fedf799f98ffaefb6fb91d77db7dc8fc8ff23fb5dc8fd77db7dc8ff3fc23f9", &a);
	hex_to_big("3ace9e4bddc3029198a2be2ef84826ea23060628308a93ec90170e02654f33df", &b);
	hex_to_big("1790f520c6645bdc6192b7da46c9382a5b9d8bf3e856a96e2c7018bc46f38534", &l);
	hex_to_big("9ac6241f", &c);
	hex_to_big("30591451fdebaf7c7c0457f47a3139c5db1bde9faa002f53134d7bb030ed3bbcebcd28b466227cc87766421df596a50c58c21d04c88ebf9ed887b58bf7112dc", &d);
	
	memcpy(p.value, P25519, sizeof(dig_t) * 8);
	memcpy(np.value, N25519, sizeof(dig_t) * 8);

	printf("Mod\n");
	start = clock();
	big_mod_25519(&d, &p, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Mult\n");
	start = clock();
    big_mul(&a, &b, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Sub\n");
	start = clock();
    big_sub(&b, &a, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Sum\n");
	start = clock();
    big_sum(&a, &b, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Power Mod\n");
	big_t tst;
	big_cpy(&p, &tst);
	tst.value[0] -= 2;
	start = clock();
	big_mnt_pow_25519(&a, &tst, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("Mod Inverse\n");
	start = clock();
    big_mod_inv(&a, &p, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("One Rshift\n");
	start = clock();
    big_rst(&a, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("32 rshift\n");
	start = clock();
    big_rst_wrd(&a, 1, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("And Gate\n");
	start = clock();
    big_and(&d, &a, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("L shift\n");
	start = clock();
    big_lst(&a, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("32 lshift\n");
	start = clock();
    big_lst_wrd(&a, 1, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(&r);
	big_null(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("legendre\n");
	start = clock();
	int x = big_lgd_sym(&a, &p);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("%d\n", x);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	ec_t curvetest;
	ec_init_c25519(curvetest);
	ecp_t PR, PR2, PR3;

	printf("curve double\n");
	start = clock();
	ecp_dbl(&curvetest, &curvetest.G, &p, &PR);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	ecp_get_afn(&PR, &p, &r);
	big_to_hex(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("curve add\n");
	start = clock();
	ecp_add(&curvetest, &PR, &curvetest.G, &p, &PR2);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	ecp_get_afn(&PR2, &p, &r);
	big_to_hex(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");

	printf("curve mult\n");
	start = clock();
	ecp_mul_cst(&curvetest, &curvetest.G, &l, &p, &PR3);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	ecp_get_afn(&PR3, &p, &r);
	big_to_hex(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");
	ecp_null(&PR3);
	big_null(&r);

	/*printf("radn num, curve mult and poly1305\n");
	start = clock();
	big_to_hex(&np);
	big_rnd_dig(&r);
	r.value[7] &= 0x7FFFFFFFull;
	while (big_gth_uns(&r, &np) > 0) {
    	big_rnd_dig(&r);
		r.value[7] &= 0x7FFFFFFFull;
	}
	ecp_mul_cst(&curvetest, &curvetest.G, &r, &p, &PR3);
	uint32_t nonce[3];
	uint32_t msg[16];
	uint32_t auth[5];
	nonce[0] = (uint32_t)r.value[7];
	nonce[1] = (uint32_t)r.value[5];
	nonce[2] = (uint32_t)r.value[3];
	ecp_get_afn(&PR3, &p, &r);
	big_to_hex(&r);

	uint32_t keykox[8];
	for (int i = 0; i < 8; i++){
		keykox[i] = (uint32_t)r.value[i];
		msg[i] = (uint32_t)d.value[i];
		msg[i + 8] = (uint32_t)d.value[i + 8];
	}
	poly1305_mac(keykox, nonce, msg, 64, auth);
	big_null(&r);
	for (int i = 0; i < 5; i++){
		r.value[i] = (dig_t)auth[i];
	}
	big_to_hex(&r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("%lf\n", cpu_time_used);
	printf("\n");*/

	return 0;
}