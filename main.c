#include "newbignum.h"

int
main(int argc, char const *argv[])
{
	big_t *a = big_new();
	big_t *c = big_new();
	bin_to_big(P224, c);
	big_t *t = big_new();
	big_t *t1 = big_new();
	bin_to_big("1", t1);
	big_sub(c, t1, t);
	big_rst(t, c);
	big_to_hex(t);
	big_to_hex(c);
	bin_to_big("11000111011111111100100011111110100011111111110011001011111111001010010100100000011111110100011111111110011001011111111001010010100100011111111110011001111010001111111111001100101111111100101001010010010100100011001111111111", a);
	big_t *sum = big_new();
	//printf("%s\n", big_to_bin(a));
	//big_fst_25519_mod(a, sum);
	big_pow(a, c, big_fst_p224_mod, sum);
	//big_fst_25519_mod(a, sum);
	big_to_hex(sum);
	big_to_hex(c);
	//big_mul(p224, p224, sum);
	big_free(a);
	big_free(sum);
	big_free(c);
	return 0;
}