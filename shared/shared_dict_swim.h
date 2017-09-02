#ifndef _shared_dict_swim_h
#define _shared_dict_swim_h

//define dictionary's reference number in the shared_dictionaries.h file
//then include this dictionary file in shared_dictionaries.h
//The dictionary number is literally used as usb transfer request field
//the opcodes and operands in this dictionary are fed directly into usb setup packet's wValue wIndex fields


//=============================================================================================
//=============================================================================================
// SWIM DICTIONARY
//
// opcodes contained in this dictionary must be implemented in firmware/source/swim.c
//
//=============================================================================================
//=============================================================================================


//activate swim on device as initiated with dict_io SWIM_INIT
//return SUCCESS if device responds with sync frame
#define SWIM_ACTIVATE	0

//hold swim pin low for 128 clocks to init comms
//return SUCCESS if device responds with sync frame
#define	SWIM_RESET	1

//SWIM commands
#define	SWIM_SRST	2	//reset device	

#define	ROTF		0x11	//read on the fly only one byte
//#define	ROTF_8B		0x18	//read on the fly RL=8
//#define	ROTF_128B	0x1F	//read on the fly RL=128 (current max due to 254B limit)

#define	WOTF		0x21	//write on the fly only one byte
//#define	WOTF_8B		0x28	//write 8Bytes on the fly
//#define	WOTF_128B	0x2F	//write 128Bytes on the fly

#endif
