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


//pullup as many cart pins as possible
//goal to be safe state for all hardware
//LED is pulled up (DIM) to help indicate this io state
//FFs are disabled due to pull up on /OE
#define IO_RESET	0

//NES cartridge interfacing setup
//set outputs as required
//latch address of $0000
//disable NES cart memories
#define NES_INIT	1

//SNES cartridge interfacing setup
//set outputs as required
//latch address of $000000
//disable cart memories
//reset high disables SRAM and puts INL carts in PRGM mode
#define SNES_INIT	2


#endif
