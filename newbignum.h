#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Macros for organization
#define MAXDIGITS		16
#define WORDSSIZE		32 * sizeof(uint64_t)
#define BASE 			0x100000000u
#define BASEM			0xFFFFFFFFu
#define LESS 			0
#define EQUAL 			1
#define GREATER 		2

// Pre-computed values
static const char P25519[] 				= "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111101101";
static const char BK_MINUS_25519[] 		= "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
static const char BK_PLUS_25519[] 		= "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
static const char BK_PLUS_25519_MINUS[] = "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
static const char U_25519[] 			= "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
static const char BETA_25519[] 			= "1101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101000011010111100101";
static const char R_25519[] 			= "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
static const char R_MINUS_25519[] 		= "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
static const char A_25519[] 			= "100110";

// Big Integer structure
typedef struct _big_t{
	uint64_t 	value[32];
	bool 		sign;
}big_t;

// Initialization, copy and free's functions
big_t* 	big_new				();
void 	big_free			(big_t *);
void 	big_null			(big_t *);
void 	big_cpy				(const big_t *, big_t *);

// Logic and arithmetic operations functions
void 	big_mul				(const big_t *, const big_t *, big_t *);
void 	big_sub				(const big_t *, const big_t *, big_t *);
void 	big_sum				(const big_t *, const big_t *, big_t *);
void	big_mont_pow		(const big_t *, const big_t *, const big_t *, big_t *, const big_t *, const big_t *, const big_t *, const big_t *, const big_t *, const big_t *, const big_t *, big_t *);
void 	big_mod_inv			(const big_t *, const big_t *, big_t *);
void	big_fst_mod			(const big_t *, const big_t *, const big_t *, const big_t *, const big_t *, const big_t *, big_t *);
void 	big_mod				(const big_t *, const big_t *, big_t *);
void 	big_rst				(const big_t *, big_t *);
void 	big_rst_word		(const big_t *, uint32_t, big_t *);
void 	big_and				(const big_t *, const big_t *, big_t *);

// Utility functions (Compare, primes...)
int 	big_gth				(const big_t *, const big_t *);
int 	big_gth_uns			(const big_t *, const big_t *);
int 	big_legendre_symbol	(const big_t *, const big_t *, big_t *, const big_t *, const big_t *, const big_t *, const big_t *, const big_t *, const big_t *, const big_t *);
void 	bin_to_big			(const char *, big_t *);
bool 	big_eql				(const big_t *, const big_t *);
void 	big_to_hex			(const big_t *);
bool 	big_odd				(const big_t *);
char* 	big_to_bin			(const big_t *);