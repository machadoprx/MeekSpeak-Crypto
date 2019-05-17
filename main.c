#include "newbignum.h"

int
main(int argc, char const *argv[])
{
	big_t *a = big_new();
	big_t *c = big_new();
	//bin_to_big(P25519, c);
	bin_to_big("1100101010010111001", c);
	bin_to_big("1101011101111101110010111011111011100100011111110101101111101110010001111111011111011100100011111110101101111101110010001111111010110111110111001000111111101011010110111110111001000111111101011011111011100100011111110101101111101110010001111111010", a);
	char *test = big_to_bin(c);
	printf("%s", test);
	free(test);
	big_t *sum = big_new();
	//printf("%s\n", big_to_bin(a));
	//big_fst_25519_mod(a, sum);
	big_pow(a, c, big_fst_25519_mod, sum);
	//big_to_hex(sum);
	//big_fst_25519_mod(a, sum);
	//int x = big_legendre_symbol(a, c, big_fst_25519_mod);
	//printf("%d\n", x);
	big_free(a);
	big_free(sum);
	big_free(c);
	return 0;
}