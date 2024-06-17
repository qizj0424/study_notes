all: modules

.PHONY: modules clean

modules:
	#mips-linux-gnu-gcc -muclibc crc32.c -o crc32
	gcc  crc32.c -o crc32
	#cp crc32 /home/zjqi/nfsroot/

clean:
	rm crc32

