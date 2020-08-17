bench:
	gcc -march=native -O3 benchmark/main.c bn/bn.c ecc/ecc_25519.c hash/hash.c utils.c -Wall -o bn_ec_test.out
aead:
	gcc -O3 -march=native ecdh_aead_example.c bn/bn.c ecc/ecc_25519.c hash/hash.c aead.c utils.c -Wall -o bn_ec_test.out
run:
	./bn_ec_test.out
