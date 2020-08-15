#ifndef AEAD_H
#define AEAD_H

#include <stdint.h>
#include "./hash/hash.h"
#include "utils.h"

void chacha20_aead_encrypt(uint32_t key[8], uint32_t nonce[3], uint8_t *aad_data, unsigned aad_len, uint8_t *plain_data, unsigned plain_len, uint8_t *cipher, uint8_t *tag);
int chacha20_aead_decrypt(uint32_t key[8], uint32_t nonce[3], uint8_t *aad_data, unsigned aad_len, uint8_t *cipher, unsigned plain_len, uint8_t *tag, uint8_t *plain_data);

#endif