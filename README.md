stm32-freertos
==============

This is a basic FreeRTOS demo for the STM32F10x.

This project is meant to be built with gcc (ex: arm-none-eabi toolchain) 
and GNU Make.

For example, to build the project:

`$ make

Both binary and hex output will be generated.

To flash the chip, assuming you are using the stm32loader python script:

`$ make install

Review the config.mk file to setup serial ports and baud rates as well
as build targets.

