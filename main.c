#include "newbignum.h"

int
main(int argc, char const *argv[])
{
	big_t *a = big_new();
	big_t *c = big_new();
	bin_to_big(P25519, c);
	//bin_to_big("1100101010010111001", c);
	bin_to_big("11010111011111111100100011111110100011111111110011001011111111001010010100100000011111110100011111111110011001011111111001010010100100011111111110011001111010001111111111001100101111111100101001010010010100111100", a);
	big_t *sum = big_new();
	//printf("%s\n", big_to_bin(a));
	//big_fst_25519_mod(a, sum);
	//big_pow(a, c, big_fst_25519_mod, sum);
	//big_to_hex(sum);
	//big_fst_25519_mod(a, sum);
	int x = big_legendre_symbol(a, c, big_fst_25519_mod);
	printf("%d\n", x);
	big_free(a);
	big_free(sum);
	big_free(c);
	return 0;
}