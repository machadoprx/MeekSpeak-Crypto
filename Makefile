all:
	gcc bn/bn.c ecc/ecc_25519.c benchmark/main.c hash/hash.c -O3 -march=native -mtune=native -pedantic -Wall -o bn_ec_test.out
run:
	./bn_ec_test.out
