all:
	gcc main.c newbignum.c -pipe -mtune=skylake -march=skylake -Ofast -Wall