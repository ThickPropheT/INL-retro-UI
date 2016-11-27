#ifndef _shared_dict_snes_h
#define _shared_dict_snes_h

//define dictionary's reference number in the shared_dictionaries.h file
//then include this dictionary file in shared_dictionaries.h
//The dictionary number is literally used as usb transfer request field
//the opcodes and operands in this dictionary are fed directly into usb setup packet's wValue wIndex fields


//=============================================================================================
//=============================================================================================
// SNES DICTIONARY
//
// opcodes contained in this dictionary must be implemented in firmware/source/snes.c
//
//=============================================================================================
//=============================================================================================


//	OPCODES with no operand and no return value besides SUCCESS/ERROR_CODE




//=============================================================================================
//	OPCODES WITH OPERAND and no return value besides SUCCESS/ERROR_CODE
//=============================================================================================
//	Detect this opcode/operand setup with opcode between the following defines:
#define SNES_OPCODE_24BOP_MIN	0x00
#define SNES_OPCODE_24BOP_MAX	0x7F
//
//=============================================================================================
//=============================================================================================


//SNES has 24bit address bus which is max operands we can send
//So have to get a little creative for writing to 24bit address
//prob easiest to just keep last latched value of A23-16

//write to SNES in program mode (/RESET low)
//only can send A15-A0
//A23-16 are last latched values
//SNES /ROMSEL always goes low
//program writes don't allow access to SRAM
//program access aligns ROM linearly and is independent of Hi/Lo switch
#define SNES_A15_A0_PRGM_WR	0x00

//write to SNES in play mode (/RESET high)
//only can send A15-A0
//A23-16 are last latched values
//SNES /ROMSEL always goes low
//play writes allow access to SRAM
//program access aligns ROM based on position of Hi/Lo switch
#define SNES_A15_A0_PLAY_WR	0x01

//write to SNES in play mode (/RESET high)
//only can send A15-A0
//A23-16 are last latched values
//SNES /ROMSEL not affected
//play writes allow access to SRAM
//program access aligns ROM based on position of Hi/Lo switch
#define SNES_A15_A0_NO_ROMSEL_PLAY_WR	0x02

//=============================================================================================
//	OPCODES WITH OPERAND 8bit RETURN VALUE plus SUCCESS/ERROR_CODE
//=============================================================================================
//	Detect this opcode/operand setup with opcode between the following defines:
#define SNES_OPCODE_24BOP_8BRV_MIN	0x80
#define SNES_OPCODE_24BOP_8BRV_MAX	0xFF
//
//=============================================================================================
//=============================================================================================

//can supply entire 24bit address

//write to SNES in program mode (/RESET low)
//SNES /ROMSEL always goes low
//program reads don't allow access to SRAM
//program access aligns ROM linearly and is independent of Hi/Lo switch
#define SNES_PRGM_RD		0x80

//write to SNES in play mode (/RESET high)
//SNES /ROMSEL always goes low
//play reads allow access to SRAM
//program access aligns ROM based on position of Hi/Lo switch
#define SNES_PLAY_RD		0x81

//write to SNES in play mode (/RESET high)
//SNES /ROMSEL not active
//play reads allow access to SRAM
//program access aligns ROM based on position of Hi/Lo switch
#define SNES_NO_ROMSEL_PLAY_RD		0x82



#endif
