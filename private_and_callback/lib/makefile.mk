all: module

.PHONY: module

module:
	mips-linux-gnu-gcc -muclibc  -c $(PWD)/lib/a.c -o lib.a


