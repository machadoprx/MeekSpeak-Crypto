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
	hex_to_big("77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a", &l);
	hex_to_big("9ac6241f", &c);
	hex_to_big("30591451fdebaf7c7c0457f47a3139c5db1bde9faacc877667c0457f47a3139c5db1b7c0457f47a3139c5db1b421df59658c21d04c88ebf9ed887b58bf7112dc", &d);
	
	memcpy(p.value, P25519, sizeof(uint32_t) * 8);
	memcpy(np.value, N25519, sizeof(uint32_t) * 8);

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

	printf("curve mult\n");
    big_t Gx;
    big_null(&Gx);
    Gx.value[0] = Gx_25519;

	/*
		if bits % 8:
		u[-1] &= (1<<(bits%8))-1
	*/
	uint8_t bytes[32];
	u32_to_u8(l.value, bytes, 8);
	big_null(&l);
	mask_bytes(bytes);
	u8_to_u32(bytes, l.value, 8);
	start = clock();
	ecp_mul_cst(&Gx, &l, &p, &r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	decode_uc(r.value);
	big_to_hex(&r);
	printf("%lf\n", cpu_time_used);
	printf("\n");
	
	return 0;
}