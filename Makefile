all:
	gcc newbignum.c main.c simple25519.c -pipe -flto -march=native -mtune=native -Ofast -pedantic