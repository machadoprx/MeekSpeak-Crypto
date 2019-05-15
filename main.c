#include "newbignum.h"

int
main(int argc, char const *argv[])
{
	big_t *a = big_new();
	big_t *c = big_new();
	bin_to_big("10101100110111101011011", c);
	bin_to_big("111111110011111111110011011010111010110100111110010100101001000111111111100110010111111111001101100101010100111110010100101001000111111111100110010111111110010100101001111100101001100110011010010001111111111001100101111111100101001010010001111111111001100", a);
	big_t *sum = big_new();
	//printf("%s\n", big_to_bin(a));
	//big_fst_25519_mod(a, sum);
	big_pow(a, c, big_fst_25519_mod, sum);
	//big_fst_25519_mod(a, sum);
	big_to_hex(sum);
	//big_mul(p224, p224, sum);
	big_free(a);
	big_free(sum);
	big_free(c);
	return 0;
}