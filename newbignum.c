#include "newbignum.h"
#include <string.h>
#define DIGIT_SIZE 32

uint64_t
bin_to_int(char *num)
{
	uint64_t res = 0;
	for (int i = 0; i < strlen(num); ++i){
		res = res << 1;
		if(num[i] == '1')
			res = res | 1;
	}
	return res;
}

char*
padd_str(char *str)
{
	char *mask = (char*)malloc(sizeof(char) * DIGIT_SIZE * DIGIT_SIZE);
	memset(mask, '0', DIGIT_SIZE * DIGIT_SIZE);
	int j = strlen(str) - 1;
	for (int i = (DIGIT_SIZE * DIGIT_SIZE) - 1; j >= 0; --i){
		mask[i] = str[j--];
	}
	return mask;
}

uint64_t*
bin_to_big(char *source)
{
	char *num = padd_str(source);
	int digits = 32;
	uint64_t *big = (uint64_t*)malloc(sizeof(uint64_t) * digits);
	for (int i = 0; i < digits; ++i){
		char *digit = (char*)malloc(sizeof(char) * DIGIT_SIZE);
		for (int j = DIGIT_SIZE * i, m = 0; j < DIGIT_SIZE * (i + 1); ++j){
			digit[m++] = num[j];
		}
		big[(digits - 1) - i] = bin_to_int(digit);
		free(digit);
	}
	return big;
}

int
main(int argc, char const *argv[])
{
	char *num = "1010111111111111110110100011101011110111111111111000101001110101100010100111010011111100101011101101000011010111111110101011101010";
	uint64_t *big = bin_to_big(num);
	free(big);
	return 0;
}