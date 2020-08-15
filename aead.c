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
    serialize(&aad_len, mac_ptr, 1);
    mac_ptr += sizeof(uint32_t);
    serialize(&plain_len, mac_ptr, 1);
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
    for (unsigned i = 0; i < 16; i++) {
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

/*int main() {
    uint8_t msg[] = "vitao sou eu mesmovitao sou eu mesmovitao sou eu mesmovitao sou";
    int len = 64;
    uint32_t key[] = {0xFFFEDu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0x7FFFFFFFu};
    uint32_t nonce[] = {0x0u, 0x4u, 0x0u};
    uint8_t cipher[len];
    uint8_t tag[16];
    memset(cipher, 0, len);
    memset(tag, 0, 16);

    printf("Original msg: ");
    printf("%s\n", (char*)msg);

    chacha20_aead_encrypt(key, nonce, NULL, 0, msg, len, cipher, tag);

    printf("tag: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", tag[i]);
    }
    printf("\n");

    uint8_t res[len];
    memset(res, 0, len);
    int auth = chacha20_aead_decrypt(key, nonce, NULL, 0, cipher, len, tag, res);
    printf("auth: %d\n", auth);
    printf("decrypted cipher: ");
    printf("%s\n", (char*)res);
    return 0;
}*/