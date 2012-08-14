stm32-freertos
==============

basic FreeRTOS demo for the STM32F10x

This project is meant to be built with gcc (ex: arm-none-eabi toolchain from
codesourcery) and the cmake framework from https://github.com/ObKo/stm32-cmake

For example, to generate the Makefile first run:

$ cmake -DSTM32_CHIP_TYPE=CL -DCMAKE_TOOLCHAIN_FILE=../gcc_stm32.cmake -DCMAKE_BUILD_TYPE=Debug

Then run:

$ make

to build the ELF image (app.elf) or:

$ make app.bin

To build a binary.
