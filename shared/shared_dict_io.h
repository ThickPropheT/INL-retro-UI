#ifndef _shared_dict_io_h
#define _shared_dict_io_h

//define dictionary's reference number in the shared_dictionaries.h file
//then include this dictionary file in shared_dictionaries.h
//The dictionary number is literally used as usb transfer request field
//the opcodes and operands in this dictionary are fed directly into usb setup packet's wValue wIndex fields


//=============================================================================================
//=============================================================================================
// IO DICTIONARY
//
// opcodes contained in this dictionary must be implemented in firmware/source/io.c
//
//=============================================================================================
//=============================================================================================





//=============================================================================================
//	OPCODES with no operand and no return value besides SUCCESS/ERROR_CODE
//=============================================================================================
//	0x00-0xFF
//	Detect this opcode/operand setup with opcode between the following defines:
#define IO_OPCODE_ONLY_MIN	0x00
#define IO_OPCODE_ONLY_MAX	0xFF
//
//=============================================================================================
//=============================================================================================

//pullup as many cart pins as possible
//goal to be safe state for all hardware
//doesn't currently contain any checks to report error/success from
//this is intended to be the "reset" safest condition for the kazzo
//LED is pulled up (DIM) to help indicate this io state
//EXP FF is disabled due to pull up on /OE
#define IO_RESET	0x00


//NES cartridge interfacing setup
//set outputs as required
//latch address of $0000
//disable NES cart memories
#define NES_INIT	0x01

//SNES cartridge interfacing setup
//set outputs as required
//latch address of $000000
//disable cart memories
//reset high disables SRAM and puts INL carts in PRGM mode
#define SNES_INIT	0x02

#endif
