all:
	gcc newbignum.c main.c simple25519.c -march=native -mtune=native -Ofast -Wall -pedantic