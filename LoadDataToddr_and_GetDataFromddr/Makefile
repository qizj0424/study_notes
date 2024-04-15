all: modules

.PHONY: modules clean

modules:
	mips-linux-gnu-gcc -muclibc a.c -o creat_test_file.bin
	mips-linux-gnu-gcc -muclibc b.c -o load_test_file_to_ddr.bin
	mips-linux-gnu-gcc -muclibc c.c -o get_test_bin_from_ddr.bin

clean:
	rm *bin

