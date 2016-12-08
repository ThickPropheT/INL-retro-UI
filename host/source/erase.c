#include "erase.h"

int erase_nes( USBtransfer *transfer ) 
{
	//uint8_t rv[8];
	//int i = 0;
	
	debug("erasing_nrom");

	io_reset(transfer);

	//for NROM flashing first verify EXP0 pull up will clock /WE properly
	check( !exp0_pullup_test(transfer), "EXP0 pullup test failed can't erase PRG-ROM" );

	nes_init(transfer);

	
	dictionary_call_debug( transfer,   DICT_NES,   DISCRETE_EXP0_PRGROM_WR,   	0x5555,   0xAA,	USB_IN, NULL, 1);
	dictionary_call_debug( transfer,   DICT_NES,   DISCRETE_EXP0_PRGROM_WR,   	0x2AAA,   0x55,	USB_IN, NULL, 1);
	dictionary_call_debug( transfer,   DICT_NES,   DISCRETE_EXP0_PRGROM_WR,   	0x5555,   0x80,	USB_IN, NULL, 1);
	dictionary_call_debug( transfer,   DICT_NES,   DISCRETE_EXP0_PRGROM_WR,   	0x5555,   0xAA,	USB_IN, NULL, 1);
	dictionary_call_debug( transfer,   DICT_NES,   DISCRETE_EXP0_PRGROM_WR,   	0x2AAA,   0x55,	USB_IN, NULL, 1);
	dictionary_call_debug( transfer,   DICT_NES,   DISCRETE_EXP0_PRGROM_WR,   	0x5555,   0x10,	USB_IN, NULL, 1);
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 
	dictionary_call_debug( transfer,   DICT_NES,   NES_CPU_RD,	0x8000,   0,	USB_IN, NULL, 2); 

	//dictionary_call( transfer,	IO,	IO_RESET,			0,		0);
	//dictionary_call( transfer,	IO,	NES_INIT,			0,		0);
	//dictionary_call( transfer,	IO,	EXP0_PULLUP_TEST,		0,		0);
	//dictionary_call( transfer,	PINPORT,	AUX_RD,		0,		0);
	//dictionary_call( transfer,	PINPORT,	AUX_RD,		0,		0);
	//dictionary_call( transfer,	PINPORT,	AUX_RD,		0,		0);
	//dictionary_call( transfer,	PINPORT,	AUX_RD,		0,		0);
	//dictionary_call( transfer,	PINPORT,	AUX_RD,		0,		0);
	//dictionary_call( transfer,	PINPORT,	AUX_RD,		0,		0);
	//dictionary_call( transfer,	PINPORT,	AUX_RD,		0,		0);

////software mode
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x5555,		0xAA);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x2AAA,		0x55);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x5555,		0x90);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8001,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8001,		0);
////exit software
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x8000,		0xF0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8001,		0);

//erase
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x5555,		0xAA);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x2AAA,		0x55);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x5555,		0x80);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x5555,		0xAA);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x2AAA,		0x55);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x5555,		0x10);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0); 
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0); 
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0); 
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0); 
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0); 
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);


	return 0;
error:
	return -1;
}
