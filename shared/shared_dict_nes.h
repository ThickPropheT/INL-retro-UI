#ifndef _shared_dict_nes_h
#define _shared_dict_nes_h

//define dictionary's reference number in the shared_dictionaries.h file
//then include this dictionary file in shared_dictionaries.h
//The dictionary number is literally used as usb transfer request field
//the opcodes and operands in this dictionary are fed directly into usb setup packet's wValue wIndex fields


//=============================================================================================
//=============================================================================================
// NES DICTIONARY
//
// opcodes contained in this dictionary must be implemented in firmware/source/nes.c
//
//=============================================================================================
//=============================================================================================


//	OPCODES with no operand and no return value besides SUCCESS/ERROR_CODE




//=============================================================================================
//	OPCODES WITH OPERAND and no return value besides SUCCESS/ERROR_CODE
//=============================================================================================
//	0x00-0x7F
//	Detect this opcode/operand setup with opcode between the following defines:
#define NES_OPCODE_24BOP_MIN	0x00
#define NES_OPCODE_24BOP_MAX	0x7F
//
//=============================================================================================
//=============================================================================================




//Discrete board PRG-ROM only write, does not write to mapper
//This is a /WE controlled write with data latched on rising edge EXP0
//PRG-ROM /WE <- EXP0 w/PU
//PRG-ROM /OE <- /ROMSEL
//PRG-ROM /CE <- GND
//PRG-ROM write: /WE & /CE low, /OE high
//mapper '161 CLK  <- /ROMSEL
//mapper '161 /LOAD <- PRG R/W
//wValueMSB: data
//wIndex: address
#define DISCRETE_EXP0_PRGROM_WR		0x00

#define NES_PPU_WR			0x01

//generic CPU write with M2 toggle as expected with NES CPU
// A15 decoded to enable /ROMSEL as it should
#define NES_CPU_WR			0x02

//=============================================================================================
//	OPCODES WITH OPERAND AND RETURN VALUE plus SUCCESS/ERROR_CODE
//=============================================================================================
//
//
#define NES_OPCODE_16BOP_8BRV_MIN	0x80
#define NES_OPCODE_16BOP_8BRV_MAX	0xFF
//
//=============================================================================================
//=============================================================================================

//read from NES CPU ADDRESS
//set /ROMSEL, M2, and PRG R/W
//read from cartridge just as NES's CPU would
//nice and slow trying to be more like the NES
#define EMULATE_NES_CPU_RD		0x80

//like the one above but not so slow..
#define NES_CPU_RD			0x81

#define NES_PPU_RD			0x82

//doesn't have operands just returns sensed CIRAM A10 mirroring 
//returns VERT/HORIZ/1SCNA/1SCNB from shared_enums.h
#define CIRAM_A10_MIRROR		0x83

#endif
