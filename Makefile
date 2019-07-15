all:
	gcc bn/bn.c ecc/ecc_25519.c benchmark/main.c -pipe -flto -march=native -mtune=native -msse -msse2 -msse3 -Ofast -Wall -o bn_ec_test.out
run:
	./bn_ec_test.out
