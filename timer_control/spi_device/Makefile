obj-m += spi_dev.o

TARGET_SYSROOT = $(HOME)/bin/poky_headers/3.0.1/sysroots/arm1176jzfshf-vfp-poky-linux-gnueabi
KBUILD_DIR := $(TARGET_SYSROOT)/lib/modules/4.19.75/build

all:
	make -C $(KBUILD_DIR) M=$(PWD) modules
clean:
	make -C $(KBUILD_DIR) M=$(PWD) clean
