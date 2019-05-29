#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Macros for organization
#define twodig_t 		__uint128_t
#define dig_t			uint64_t
#define MAX_DIGITS		16
#define WORDS_SIZE		22 * sizeof(dig_t)
#define BASE 			0x100000000ull
#define BASE_M			0xFFFFFFFFull
#define DIGIT_BITS		32
#define LESS 			0
#define EQUAL 			1
#define GREATER 		2

// Big Integer structure
typedef struct _big_t{
	dig_t	value[22];
	bool 	sign;
}big_t;

// Initialization, copy and free's functions
big_t* 	big_new				();
void 	big_free			(big_t *);
void 	big_null			(big_t *);
void 	big_cpy				(big_t *, big_t *);

// Logic and arithmetic operations functions
void 	big_mul				(big_t *, big_t *, big_t *);
void	big_sqr				(big_t *, big_t *);
void 	big_sub				(big_t *, big_t *, big_t *);
void 	big_sum				(big_t *, big_t *, big_t *);
void	big_mont_pow		(big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *);
void 	big_mod_inv			(big_t *, big_t *, big_t *);
void	big_barrett_mod		(big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *);
void 	big_mod				(big_t *, big_t *, big_t *);
void	big_lst				(big_t *, big_t *);
void 	big_rst				(big_t *, big_t *);
void 	big_rst_word		(big_t *, int, big_t *);
void 	big_lst_word		(big_t *, int, big_t *);
void 	big_and				(big_t *, big_t *, big_t *);

// Utility functions (Compare, primes...)
int 	big_gth				(big_t *, big_t *);
int 	big_gth_uns			(big_t *, big_t *);
int 	big_legendre_symbol	(big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *, big_t *);
bool 	big_eql				(big_t *, big_t *);
void 	big_to_hex			(big_t *);
char* 	big_to_bin			(big_t *);
void 	bin_to_big			(char *, big_t *);
void	hex_to_big			(char *, big_t *);