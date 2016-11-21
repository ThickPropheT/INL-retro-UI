# Name: Makefile
# Project: custom-class example
# Author: Christian Starkjohann
# Creation Date: 2008-04-07
# Tabsize: 4
# Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)

DEVICE  = atmega164a
F_CPU   = 16000000	# in Hz
# Fuses and ISP programming handled in bootloader firmware build

SOURCES=$(wildcard source/**/*.c source/*.c)
CFLAGS  = -Iusbdrv -Isource -DDEBUG_LEVEL=0
USBOBJ = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o
OBJECTS=$(patsubst %.c,%.o,$(SOURCES)) $(USBOBJ)

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)

# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ............ to build main.hex"
	@echo "make program ........ windows flash firmware via bootloader"
	@echo "make program_unix ... unix flash firmware via bootloader"
	@echo "make clean .......... to delete objects and hex file"

hex: main.hex

program: main.hex 
	../bootloader/commandline/bootloadHID.exe -r $<

program_unix: main.hex
	../bootloader/commandline/bootloadHID -r $<

# rule for deleting dependent files (those which can be built by Make):
clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.elf $(OBJECTS) main.s usbdrv/oddebug.s usbdrv/usbdrv.s

# Generic rule for compiling C files:
.c.o:
	$(COMPILE) -c $< -o $@

# Generic rule for assembling Assembler source files:
.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

# file targets:

main.elf: shared $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size main.hex

# debugging targets:

disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c

#copy shared .h files which are used in host and firmware
shared:
	cp -r ../shared/* source/
