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
#define IO_OPCODE_ONLY_MAX	0x7F
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


//=============================================================================================
//	OPCODES with no operand but have RETRUN VALUE plus SUCCESS/ERROR_CODE
//=============================================================================================
//	0x00-0xFF
//	Detect this opcode/operand setup with opcode between the following defines:
#define IO_OPCODE_RTN_MIN	0x80
#define IO_OPCODE_RTN_MAX	0xFF
//
//=============================================================================================
//=============================================================================================

//Test EXP0 drive with pull up 
//This is an important test if reling on pulling up EXP0 pin to drive the cart's pin.
//EXP0 is used for various things and may have pull up/down of it's own or significant load
//Test that programmer can drive EXP0 with a pull up before reling on commands that only pullup
//If this fails, can resort to driving EXP0 with SNES /RESET commands but excersize caution
//as some NES cart's have CPLD JTAG TDO pin placed on EXP0 that may not be 5v tolerant.
//FC also has APU audio placed on EXP0, carts without exp sound short RF sound (EXP6) to EXP0
//So if EXP FF's output is enabled, driving the mcu EXP0 pin could cause damage to the pin.
//that's why it's best in general to only pull up EXP0 instead of driving it high.
//Have to drive it low however to get logic 0 most times.  If famicom cart is inserted,
//it's probably safer to drive EXP0 through EXP6 sound jumper and leave EXP0 floating/pulledup.
//
//Test starts by verifying EXP0 can be driven low, if not, will return one byte of AUX_PIN
//followed by alternating 0xAA, 0x55, 0xAA...
//This test pulls up EXP0 and then reads AUX_PIN 6 times in rapid succession returning error code
//plus 6 bytes of read data.  If pull up works but is just slow, should see that in return data.
//data[0] marks bit where EXP0 resisdes to provide host with bitmask for EXP0
#define EXP0_PULLUP_TEST	0x80
//testing results:
//without any cart inserted takes 5 cycles for EXP0 to settle high
//er: 0 rv: 1, b8, b8, b8, b8, b9, b9
//with discrete NROM board inserted (has pullup on EXP0) settled immediately
//er: 0 rv: 1, 99, 99, 99, 99, 99, 99
//SNES board inserted never settled
//er: 0 rv: 1, b8, b8, b8, b8, b8, b8
//famicom NROM choplifter cart inserted never settled
//er: 0 rv: 1, 98, 98, 98, 98, 98, 98
//INLXO-ROM board JTAG TDO (non-5v tolerant) tied to EXP0 settled immediately
//er: 0 rv: 1, 99, 99, 99, 99, 99, 99

#endif
