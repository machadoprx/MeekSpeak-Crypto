#include "newbignum.h"
#include <time.h>

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

	bin_to_big(U_25519, u);
	bin_to_big(BETA_25519, beta);
	beta->sign = true;
	bin_to_big(R_25519, R);
	bin_to_big(A_25519, A);
	bin_to_big(R_MINUS_25519, Rm);
	bin_to_big(BK_PLUS_25519_MINUS, bk_plus_minus);
	bin_to_big(BK_MINUS_25519, bk_minus);
	bin_to_big(BK_PLUS_25519, bk_plus);
	bin_to_big(P25519, p);
	bin_to_big(U_25519, u);
	bin_to_big("11010111111110110111110111100110011111100110001111111110101110111110110110111110111001000111010111011111011011011111011100100011111100100011111111001000111111101101011101110010001111110101110111110110110111110111001000111111110011111111000010001111111001", a);
	bin_to_big("11101011001110100111100100101111011101110000110000001010010001100110001010001010111110001011101111100001001000001001101110101000100011000001100000011000101000001100001000101010010011111011001001000000010111000011100000001001100101010011110011001111011111", b);
	bin_to_big("1001011100111100101", c);
	bin_to_big("111010110011101001111001001011110111011100001100000010100100011001100010100010101111100010111011111000010010000010011011101010001000110000011000000110001010000011000010001010100100111110110010010000000101110100101001001010100001110000000110010010101010001001100101010011110011001111011111", d);

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
	big_mont_pow_25516(a, c, p, A, R, beta, Rm, u, bk_minus, bk_plus, bk_plus_minus, r);
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
	big_fst_25519_mod(d, p, u, bk_minus, bk_plus, bk_plus_minus, r);
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

	printf("32 shift\n");
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
	return 0;
}