
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

INCDIRS  = ./src \
           ./lib/CMSIS/Include \
           ./lib/CMSIS/ST \
           ./src/FreeRTOS/Source/portable/GCC/ARM_CM0 \
           ./src/FreeRTOS/Source/portable/MemMang \
           ./src/FreeRTOS/Source/include
INCDIRS  := $(patsubst %,-I%, $(INCDIRS))

OUT_NAME = $(OUTDIR)/$(PROJECT)

# Sources
SRC := $(wildcard src/*.c) \
       lib/CMSIS/ST/system_stm32f0xx.c \
       $(wildcard src/FreeRTOS/Source/*.c) \
       $(wildcard src/FreeRTOS/Source/portable/GCC/ARM_CM0/*.c) \
       $(wildcard src/FreeRTOS/Source/portable/MemMang/*.c)

# Objects
OBJ = $(SRC:%.c=%.o) lib/CMSIS/ST/startup_stm32f051x8.o
LST = $(OBJ:%.o=%.lst)

LIB_DIR = /usr/lib/arm-none-eabi/lib/thumb/v6-m/nofp

# Flags
MCU  = cortex-m0
MCFLAGS = -mcpu=$(MCU)
DEBUG_FLAGS = -gdwarf-2
ASFLAGS = $(MCFLAGS) $(DEBUG_FLAGS) -mthumb  -Wa,-amhls=$(<:%.s=%.lst)
CFLAGS = $(MCFLAGS) -D $(MCU_DEFINE) $(DEBUG_FLAGS) -O0 -falign-functions=4 -mthumb -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:%.c=%.lst) $(DEFS)
LDFLAGS = $(MCFLAGS) $(DEBUG_FLAGS) -mthumb -nostartfiles --specs=nosys.specs --specs=nano.specs -Wl,-Map=$(OUTDIR)/$(PROJECT).map,--cref,--no-warn-mismatch,--gc-sections -L$(LIB_DIR)


# Rules
all : $(OBJ) $(OUT_NAME).elf $(OUT_NAME).hex $(OUT_NAME).bin
	@echo "All targets up to date."

flash: all
	st-flash write $(OUT_NAME).bin 0x8000000

%.elf : $(OBJ) $(LINKER_SCRIPT)
	$(CC) $(LDFLAGS) -T$(LINKER_SCRIPT) $(OBJ) -o $@

%.hex: %.elf
	$(HEX) $< $@

%.bin: %.elf
	$(BIN) $< $@

%.o : %.c
	$(CC) -c $(CFLAGS) $(INCDIRS) $< -o $@

%.o : %.s
	$(AS) -c $(ASFLAGS) $< -o $@

clean :
	rm -f $(OBJ)
	rm -f $(LST)
	rm -rf $(OUTDIR)/$(PROJECT).{elf,hex,bin,map}

# Create output directories if they don't already exist.
$(shell mkdir -p $(OUTDIR))
