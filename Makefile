all:
	gcc main.c newbignum.c -pipe -m64 -march=skylake -Ofast -Wall