
# 1. 使用不同的开发板内核时, 一定要修改KERN_DIR
# 2. KERN_DIR中的内核要事先配置、编译, 为了能编译内核, 要先设置下列环境变量:
# 2.1 ARCH,          比如: export ARCH=arm64
# 2.2 CROSS_COMPILE, 比如: export CROSS_COMPILE=aarch64-linux-gnu-
# 2.3 PATH,          比如: export PATH=$PATH:/home/book/100ask_roc-rk3399-pc/ToolChain-6.3.1/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin 
# 注意: 不同的开发板不同的编译器上述3个环境变量不一定相同,
#       请参考各开发板的高级用户使用手册

CROSS_COMPILE ?= mips-linux-gnu-
MODULE_NAME := driver-demo

EXTRA_CFLAGS += -I$(PWD)/inc

$(MODULE_NAME)-objs = src/driver-demo-libfunc.o \
		      src/driver-demo-device.o \
		      src/driver-demo-vic.o \
	              src/driver-demo-core.o \
	              src/driver-demo-fs.o \
	              src/driver-demo-fs1.o \

obj-m:=$(MODULE_NAME).o


CURRENT_PATH           :=$(shell pwd)

LINUX_KERNEL           :=$(shell uname -r)

#LINUX_KERNEL_PATH      :=/lib/modules/$(LINUX_KERNEL)/build
LINUX_KERNEL_PATH 	:= ${ISVP_ENV_KERNEL_DIR}
#KERN_DIR = /home/book/100ask_roc-rk3399-pc/linux-4.4

all:

	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules 
	
	$(CROSS_COMPILE)gcc -o test test.c 
	@cp driver-demo.ko /home/zjqi/nfsroot -v

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
	rm -rf modules.order
	rm -f test

