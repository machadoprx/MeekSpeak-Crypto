all:
	gcc bn/bn.c ecc/ecc_25519.c benchmark/main.c hash/hash.c -pipe -flto -march=native -mtune=native -msse -msse2 -msse3 -O3 -Wall -o bn_ec_test.out
run:
	./bn_ec_test.out
