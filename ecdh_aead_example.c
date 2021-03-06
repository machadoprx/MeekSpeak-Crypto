#include "./bn/bn.h"
#include "./ecc/ecc_25519.h"
#include "./aead.h"
#include <time.h>

int main() {

    big_t prime, priv1, priv2;
    big_t own_pbk, own_pbk2, own_pbk3, own_pbk4;

    big_null(&priv1);
    big_null(&priv2);

    get_scalar25519(priv1.value);
    get_scalar25519(priv2.value);

    printf("\t\t\t --- ECDH - Key Agreement ---\nPrivate key1: ");
	big_to_hex(&priv1);
	printf("Private key2: ");
	big_to_hex(&priv2);
	printf("\n");
    big_null(&prime);
    memcpy(prime.value, P25519, sizeof(uint32_t) * 8);

    big_t Gx;
    big_null(&Gx);
    Gx.value[0] = Gx_25519;
    ecp_mul_cst(&Gx, &priv1, &prime, &own_pbk);

    ecp_mul_cst(&own_pbk, &priv2, &prime, &own_pbk2);
    decode_uc(own_pbk2.value);

    ecp_mul_cst(&Gx, &priv2, &prime, &own_pbk3);

    ecp_mul_cst(&own_pbk3, &priv1, &prime, &own_pbk4);
    decode_uc(own_pbk4.value);

	printf("Public Key1: ");
    big_to_hex(&own_pbk);
    printf("Public Key2: ");
    big_to_hex(&own_pbk3);
    printf("\nVerifying shared secret...\n");

    uint32_t err = 0;
    for (int i = 0; i < 8; i++)
        err += own_pbk2.value[i] ^ own_pbk4.value[i];
    if (err == 0) {
        printf("Equal secret: ");
    }
    else {
        printf("Error\n");
        exit(1);
    }
    big_to_hex(&own_pbk2);
    printf("\n");
    
    printf("\t\t\t --- AEAD - ChaCha20 Encrypt ---\n");

    uint8_t msg[] = "vitao sou eu mesmovitao sou eu mesmovitao sou eu mesmovitao sou";
    int len = 64;
    uint32_t *key = own_pbk2.value;
    uint32_t nonce[] = {0x0u, 0x4u, 0x0u};
    uint8_t cipher[len];
    uint8_t tag[17];
    memset(cipher, 0, len);
    memset(tag, 0, 17);

    printf("Original msg: ");
    printf("%s\n", (char*)msg);

    chacha20_aead_encrypt(key, nonce, NULL, 0, msg, len, cipher, tag);

    printf("MAC tag: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", tag[i]);
    }
    printf("\n");

    uint8_t res[len];
    memset(res, 0, len);
    int auth = chacha20_aead_decrypt(key, nonce, NULL, 0, cipher, len, tag, res);
    printf("Auth: %s\n", auth == 0 ? "Ok" : "Not authenticated");
    printf("Decrypted cipher: %s\n", (char*)res);
    return 0;
}