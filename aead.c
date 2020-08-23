#include "aead.h"

static inline uint32_t
pad_len(unsigned len)
{
    return (len % 16 == 0) ? 0 : (16 - (len % 16));
}

void
make_mac_data(uint8_t *aad_data, unsigned aad_len, uint8_t *cipher, unsigned plain_len, unsigned mac_len, uint8_t *mac_data)
{
    memset(mac_data, 0, mac_len * sizeof(uint8_t));
    unsigned i;
    uint8_t *mac_ptr = mac_data;
    for (i = 0; i < aad_len; i++, mac_ptr++) {
        *mac_ptr = aad_data[i];
    }
    mac_ptr += pad_len(aad_len);
    for (i = 0; i < plain_len; i++, mac_ptr++) {
        *mac_ptr = cipher[i];
    }
    mac_ptr += pad_len(plain_len);
    u32_to_u8(&aad_len, mac_ptr, 1);
    mac_ptr += sizeof(uint32_t);
    u32_to_u8(&plain_len, mac_ptr, 1);
}

void
chacha20_aead_encrypt(uint32_t key[8], uint32_t nonce[3], uint8_t *aad_data, unsigned aad_len, uint8_t *plain_data, unsigned plain_len, uint8_t *cipher, uint8_t *tag)
{
    chacha_enc(key, nonce, plain_data, cipher, plain_len);

    uint32_t mac_len = aad_len + pad_len(aad_len) 
                        + plain_len + pad_len(plain_len) + (2 * sizeof(uint32_t));
    uint8_t mac_data[mac_len];
    make_mac_data(aad_data, aad_len, cipher, plain_len, mac_len, mac_data);
    poly1305_mac(key, nonce, mac_data, mac_len, tag);
}

int
chacha20_aead_decrypt(uint32_t key[8], uint32_t nonce[3], uint8_t *aad_data, unsigned aad_len, uint8_t *cipher, unsigned plain_len, uint8_t *tag, uint8_t *plain_data)
{
    uint8_t tag_test[17];

    uint32_t mac_len = aad_len + pad_len(aad_len) 
                        + plain_len + pad_len(plain_len) + (2 * sizeof(uint32_t));
    uint8_t mac_data[mac_len];
    make_mac_data(aad_data, aad_len, cipher, plain_len, mac_len, mac_data);
    poly1305_mac(key, nonce, mac_data, mac_len, tag_test);

    int cmp = 0;
    for (unsigned i = 0; i < 17; i++) {
        cmp += tag_test[i] ^ tag[i];
    }
    if (cmp != 0) {
        return -1;
    }
    else {
        chacha_enc(key, nonce, cipher, plain_data, plain_len);
        return 0;
    }
}