bench:
	gcc -O3 -march=native bn/bn.c ecc/ecc_25519.c hash/hash.c benchmark/main.c utils.c -Wall -o bn_ec_test.out
run:
	./bn_ec_test.out
