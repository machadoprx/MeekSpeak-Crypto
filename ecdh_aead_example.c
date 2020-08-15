#include "./bn/bn.h"
#include "./ecc/ecc_25519.h"
#include "./aead.h"
#include <time.h>

int main() {

    big_t prime, priv1, priv2, res1, res2;
    ecp_t own_pbk, own_pbk2, own_pbk3, own_pbk4;

    big_rnd_dig(&priv1);
	big_rnd_dig(&priv2);

    printf("\t\t\t --- ECDH - Key Agreement ---\nPrivate key1: ");
	big_to_hex(&priv1);
	printf("Private key2: ");
	big_to_hex(&priv2);
	printf("\n");
    big_null(&prime);
    memcpy(prime.value, P25519, sizeof(uint32_t) * 8);

    ec_t curve;
    ec_init_c25519(curve);
    
    ecp_mul_cst(&curve, &curve.G, &priv1, &prime, &own_pbk);
    ecp_mul_cst(&curve, &own_pbk, &priv2, &prime, &own_pbk2);
    
    ecp_mul_cst(&curve, &curve.G, &priv2, &prime, &own_pbk3);
    ecp_mul_cst(&curve, &own_pbk3, &priv1, &prime, &own_pbk4);

    ecp_get_afn(&own_pbk2, &prime, &res1);
    ecp_get_afn(&own_pbk4, &prime, &res2);
	printf("Public Key1: ");
    big_to_hex(&own_pbk.X);
    printf("Public Key2: ");
    big_to_hex(&own_pbk3.X);
    printf("\nVerifying shared secret...\n");

    uint32_t err = 0;
    for (int i = 0; i < 8; i++)
        err += res1.value[i] ^ res2.value[i];
    if (err == 0) {
        printf("Equal secret: ");
    }
    else {
        printf("Error\n");
        exit(1);
    }
    big_to_hex(&res1);
    printf("\n\n");
    
    printf("\t\t\t --- AEAD - ChaCha20 Encrypt ---\n");

    uint8_t msg[] = "vitao sou eu mesmovitao sou eu mesmovitao sou eu mesmovitao sou";
    int len = 64;
    uint32_t *key = res1.value;
    uint32_t nonce[] = {0x0u, 0x4u, 0x0u};
    uint8_t cipher[len];
    uint8_t tag[16];
    memset(cipher, 0, len);
    memset(tag, 0, 16);

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