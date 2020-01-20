
# path to environment setup script
ENV_INIT = ~/bin/poky/3.0.1/environment-setup-arm1176jzfshf-vfp-poky-linux-gnueabi
SHELL = /bin/bash

SRCDIR = src
OBJDIR = obj
OUTDIR = out
INCDIR = $(SRCDIR)


OUTNAME = libbcm2835_spi
INC_OUT = bcm2835_spi.h # headers to copy to output
INC_EXPORT = $(INC_OUT:%=$(INCDIR)/%)

WFLAGS = -Wall
CFLAGS = $(WFLAGS) 

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all : $(OBJ)
	$(AR) rcs $(OUTDIR)/libbcm2835_spi.a $(OBJ)
	mkdir -p $(OUTDIR)/include
	cp $(INC_EXPORT) $(OUTDIR)/include/

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean : 
	rm -f $(OBJ)
	rm -f $(OUTDIR)/$(OUTNAME)*
	rm -f $(OUTDIR)/include/*
	rmdir $(OUTDIR)/include
	rmdir $(OUTDIR) $(OBJDIR)

$(shell mkdir -p $(OBJDIR) $(OUTDIR))
