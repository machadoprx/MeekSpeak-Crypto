all:
	gcc newbignum.c main.c -pipe -m64 -march=skylake -mtune=skylake -Ofast -g -Wall