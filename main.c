#include "newbignum.h"

int
main(int argc, char const *argv[])
{
	big_t *a = big_new();
	big_t *c = big_new();
	//bin_to_big(P25519, c);
	//big_t *base = big_new();
	//bin_to_big(P224, base);
	bin_to_big("10101100110111101011011", c);
	bin_to_big("11010111111110110111110111100110011111100110001111111110101110111110110110111110111001000111010111011111011011011111011100100011111100100011111111001000111111101101011101110010001111110101110111110110110111110111001000111111001000111111110000100011111110", a);
	big_t *sum1 = big_new();
	//printf("%s\n", big_to_bin(a));
	//big_fst_25519_mod(a, sum);
	big_pow(a, c, big_fst_25519_mod, sum1);
	//big_mont_pow_25516(a, c, sum1);
	//big_mod_inv(a, base, big_fst_p224_mod, sum);
	//big_to_hex(sum);
	big_to_hex(sum1);
	//big_fst_25519_mod(a, sum);
	//int x = big_legendre_symbol(a, c, big_fst_25519_mod);
	//printf("%d\n", x);
	big_free(sum1);
	big_free(a);
	big_free(c);
	return 0;
}