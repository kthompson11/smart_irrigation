obj-m += spi0.o
spi0-objs = spi_dev.o bcm2835_spi.o

TARGET_SYSROOT = $(HOME)/bin/poky/3.0.1/sysroots/arm1176jzfshf-vfp-poky-linux-gnueabi
KBUILD_DIR := $(TARGET_SYSROOT)/lib/modules/4.19.75/build

INC_DIRS = -I/home/kt/dev/projects/irrigation_timer/irrigation_timer/timer_control/bcm2835_spi/out/include
ccflags-y = -Wno-declaration-after-statement $(INC_DIRS)
ldflags-y = -L/home/kt/dev/projects/irrigation_timer/irrigation_timer/timer_control/bcm2835_spi/out -lbcm2835_spi

all:
	make -C $(KBUILD_DIR) M=$(PWD) modules
clean:
	make -C $(KBUILD_DIR) M=$(PWD) clean
