include lib/makefile.mk

all: modules

.PHONY: modules clean

modules:
	mips-linux-gnu-gcc -muclibc  main.c lib.a -o test.bin

clean:
	rm *bin *a

