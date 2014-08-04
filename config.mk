## REGION: GLOBAL

# Genral Project Information
PROJNAME = TRACKER

CHIP=F1
PORTABLE=ARM_CM3

ifeq ($(CHIP), F1)
CHIPDEF = STM32F10X_MD
STARTUP = startup_stm32f10x_md.s
LD_SCRIPT = stm32f10x_flash_md.ld
endif
ifeq ($(CHIP), F4)
## PUT OTHER CHIP DEFINITIONS HERE
endif


# Build and depolyment method
PROFILE = DEBUG
SERIAL = /dev/ttyUSB0
BAUD = 115200

# Project Root
TOP := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

# Build Utilities
GENCONFIG=$(TOP)scripts/genconfig.py
FLASH=sudo python2 $(TOP)scripts/stm32loader.py -p /dev/ttyUSB0 -b $(BAUD)

## END REGION: GLOBAL


## REGION: TOOLCHAIN

# Tool Chain
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
OC = arm-none-eabi-objcopy
GDB = arm-none-eabi-gdb
STRIP = arm-none-eabi-strip

# Buid Flags
#CFLAGS = -Wall -mlittle-endian -mthumb -nostdlib -ffreestanding -mcpu=cortex-m3 -DUSE_STDPERIPH_DRIVER -D$(CHIPDEF)
CFLAGS = -Wall -mlittle-endian -mthumb -ffreestanding -mcpu=cortex-m3 -DUSE_STDPERIPH_DRIVER -D$(CHIPDEF)
ASFLAGS =
LDFLAGS = $(CFLAGS) -Wl,-T$(LD_SCRIPT)

# END REGION: TOOLCHAIN


