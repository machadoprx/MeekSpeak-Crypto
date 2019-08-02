all:
	gcc bn/bn.c ecc/ecc_25519.c benchmark/main.c hash/hash.c -Ofast -pipe -flto -march=native -mtune=native -Wall -o bn_ec_test.out
run:
	./bn_ec_test.out
