
SHELL = bash

# Project Name
PROJECT = valve_control
# Target MCU (must match a device macro name in "stm32f0xx.h")
MCU_DEFINE = STM32F051x8

OBJDIR = obj
SRCDIR = src
OUTDIR = out

LINKER_SCRIPT = linker/stm32f051.ld

CFLAGS = -D $(MCU_DEFINE)
CCPREFIX = arm-none-eabi-
CC   = $(CCPREFIX)gcc
CP   = $(CCPREFIX)objcopy
AS   = $(CCPREFIX)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S

INCDIRS  = ./lib/CMSIS/Include
INCDIRS += ./lib/CMSIS/ST
INCDIRS  := $(patsubst %,-I%, $(INCDIRS))

OUT_NAME = $(OUTDIR)/$(PROJECT)

# Sources
SRC  = src/main.c src/system_timer.c src/valve_control.c src/valve_messaging.c lib/CMSIS/ST/system_stm32f0xx.c

# Objects
OBJ = $(SRC:%.c=%.o) lib/CMSIS/ST/startup_stm32f051x8.o
LST = $(OBJ:%.o=%.lst)

# Flags
MCU  = cortex-m0
MCFLAGS = -mcpu=$(MCU)
DEBUG_FLAGS = -g -gdwarf-2
ASFLAGS = $(MCFLAGS) $(DEBUG_FLAGS) -mthumb  -Wa,-amhls=$(<:%.s=%.lst)
CFLAGS = $(MCFLAGS) -D $(MCU_DEFINE) $(DEBUG_FLAGS) -mthumb -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:%.c=%.lst) $(DEFS)
LDFLAGS = $(MCFLAGS) $(DEBUG_FLAGS) -mthumb --specs=nosys.specs -Wl,-Map=$(OUTDIR)/$(PROJECT).map,--cref,--no-warn-mismatch


# Rules
all : $(OBJ) $(OUT_NAME).elf $(OUT_NAME).hex $(OUT_NAME).bin
	@echo "All targets up to date."

flash: all
	st-flash write $(OUT_NAME).bin 0x8000000

%.elf : $(OBJ)
	$(CC) $(LDFLAGS) -T$(LINKER_SCRIPT) $(OBJ) -o $@

%.hex: %.elf
	$(HEX) $< $@

%.bin: %.elf
	$(BIN)  $< $@

%.o : %.c
	$(CC) -c $(CFLAGS) $(INCDIRS) $^ -o $@

%.o : %.s
	$(AS) -c $(ASFLAGS) $^ -o $@

clean :
	rm -f $(OBJ)
	rm -f $(LST)
	rm -rf $(OUTDIR)/$(PROJECT).{elf,hex,bin,map}

# Create output directories if they don't already exist.
$(shell mkdir -p $(OUTDIR))
