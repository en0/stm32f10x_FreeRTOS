MAKE_CONFIG = config.mk

include $(MAKE_CONFIG)


## REGION: OUTPUT

OUT=BL_$(PROJNAME)_$(CHIPDEF)

## END REGION: OUTPUT


## REGION: SOURCE

#LIBS_C=$(wildcard lib/sys/*.c) lib/device/misc.c
#LIBS_S=$(wildcard lib/sys/*.s)

LIBS_C = $(wildcard lib/device/*.c) \
         $(wildcard lib/sys/*.c)
LIBS_S = lib/sys/$(STARTUP)

APP_C = $(wildcard app/*.c)

OS_C = $(wildcard os/*.c) \
       $(wildcard os/portable/GCC/$(PORTABLE)/*.c) \
       os/portable/MemMang/heap_3.c

SRV_C = $(wildcard services/*.c)

OBJS=$(patsubst %.c, %.c.o, $(wildcard *.c) $(wildcard drivers/*.c) $(APP_C) $(OS_C) $(LIBS_C) $(SRV_C)) \
     $(patsubst %.s, %.s.o, $(wildcard *.s) $(LIBS_S)) \

#CONFIG_H=$(patsubst inc/%.h.in, inc/%.h, $(wildcard inc/*.h.in))

## END REGION: SOURCE


## REGION: FLAGS

## Determ Profile
ifeq ($(PROFILE), DEBUG)
CFLAGS += -ggdb
ASFLAGS += -ggdb
endif

## Includes
CFLAGS += -Ilib/device/inc -Ilib/sys/inc -Iinc -Ios/include -Idrivers/inc -Ios/portable/GCC/$(PORTABLE) -Iapp/inc

## END REGION: FLAGS


## REGION: BUILD

.PHONY : all

all : $(OUT).elf $(OUT).hex $(OUT).bin

$(OUT).elf : $(OBJS)
	@echo \ - LD $@ 
	@$(CC) $(LDFLAGS) -o $@ $^

%.bin : %.elf
	@echo \ - OC $@
	@$(OC) -O binary $^ $@

%.hex : %.elf
	@echo \ - OC $@
	@$(OC) -O ihex $^ $@

%.c.o : %.c
	@echo \ - CC $@ 
	@$(CC) $(CFLAGS) -o $@ -c $^

%.s.o : %.s
	@echo \ - AS $@
	@$(CC) $(CFLAGS) -o $@ -c $^

%.h : %.h.in
	@echo \ - GENCONFIG $@
	@$(GENCONFIG) $(MAKE_CONFIG) $< $@

#$(OBJS) : $(CONFIG_H)

## END REGION: BUILD


## REGION: CLEAN

OBJS_CLEAN=$(OBJS:%=clean-%) \
           $(CONFIG_H:%=clean-%) \
           $(OUT:%=clean-%).elf \
           $(OUT:%=clean-%).hex \
           $(OUT:%=clean-%).bin

.PHONY : clean $(OBJS_CLEAN)
.PHONY : clean-elf clean-hex clean-bin

clean : $(OBJS_CLEAN)

clean-elf : $(OUT:%=clean-%).elf

clean-hex : $(OUT:%=clean-%).hex

clean-bin : $(OUT:%=clean-%).bin

$(OBJS_CLEAN) :
	@echo \ - RM $(@:clean-%=%)
	@$(RM) $(@:clean-%=%)

## END REGION: CLEAN


## REGION: FLASH

.PHONY : flash install

install : flash

flash : $(OUT).bin
	@echo \ - FLASH $(SERIAL)
	@echo
	@echo Please activate flash mode:
	@echo  1. press USER_BUTTON and RESET. 
	@echo  2. Release RESET witout releasing USER_BUTTON
	@echo  3. Release USER_BUTTON.
	@echo
	@read -p "Press [ENTER] once ready"
	@$(FLASH) -ewv $^

## END REGION: FLASH


## REGION: INFO

.PHONY : info

info :
	@echo
	@echo Build Configuration Information.
	@echo
	@echo Objects for \"$(CHIPDEF) \($(PROFILE)\)\":
	@echo $(OBJS) $(OUT).elf $(OUT).hex $(OUT).bin | xargs -n1 | sed s/^/\ -\ /
	@echo
	@echo Configuration Headers.
	@echo $(CONFIG_H) | xargs -n1 | sed s/^/\ -\ /
	@echo
	@echo Cross Compiler with respective flags:
	@echo \ - CC: $(CC)
	@echo \ - CFLAGS:
	@echo $(CFLAGS) | xargs -n3 | sed s/^/\	/
	@echo
	@echo \ - AS: $(CC)
	@echo \ - ASFLAGS:
	@echo $(ASFLAGS) | xargs -n3 | sed s/^/\	/
	@echo
	@echo \ - LD: $(CC)
	@echo \ - LDFLAGS:
	@echo $(LDFLAGS) | xargs -n3 | sed s/^/\	/
	@echo

## END REGION: INFO


