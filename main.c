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
	big_t *p = big_new();
	big_t *r = big_new();

	bin_to_big("11010111111110110111110111100110011111100110001111111110101110111110110110111110111001000111010111011111011011011111011100100011111100100011111111001000111111101101011101110010001111110101110111110110110111110111001000111111110011111111000010001111111001", a);
	bin_to_big("11101011001110100111100100101111011101110000110000001010010001100110001010001010111110001011101111100001001000001001101110101000100011000001100000011000101000001100001000101010010011111011001001000000010111000011100000001001100101010011110011001111011111", b);
	bin_to_big("1001011100111100101", c);
	bin_to_big(P25519, p);

	printf("Mult\n");
	start = clock();
    big_mul(a, b, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);

	printf("Sub\n");
	start = clock();
    big_sub(b, a, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);

	printf("Sum\n");
	start = clock();
    big_sum(a, b, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);

	printf("Power Mod\n");
	start = clock();
    big_mont_pow_25516(a, c, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);

	printf("Mod Inverse\n");
	start = clock();
    big_mod_inv(a, p, big_fst_25519_mod, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);

	printf("Mod\n");
	start = clock();
    big_fst_25519_mod(a, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);

	printf("One Rshift\n");
	start = clock();
    big_rst(a, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);

	printf("32 shift\n");
	start = clock();
    big_rst_word(a, 1, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);

	printf("And Gate\n");
	start = clock();
    big_and(a, b, r);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	big_to_hex(r);
	big_null(r);
	printf("%lf\n", cpu_time_used);


	big_free(a);
	big_free(b);
	big_free(c);
	big_free(p);
	big_free(r);
	return 0;
}