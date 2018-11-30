#ifndef _shared_dict_bootload_h
#define _shared_dict_bootload_h

//define dictionary's reference number in the shared_dictionaries.h file
//then include this dictionary file in shared_dictionaries.h
//The dictionary number is literally used as usb transfer request field
//the opcodes and operands in this dictionary are fed directly into usb setup packet's wValue wIndex fields


//=============================================================================================
//=============================================================================================
// BOOTLOAD DICTIONARY
//
// opcodes contained in this dictionary must be implemented in firmware/source/bootload.c
//
//=============================================================================================
//=============================================================================================


//BOOTLOAD opcodes
#define		JUMP_BL		1	//jump to the bootloader

#define		LOAD_ADDRH	2	//upper address of jump 
#define		JUMP_ADDR	3	//jump to address upper 16bit provided previous opcode

#define		PREP_FWUPDATE	4	//leave main application and sets up for fwupdate


#endif
