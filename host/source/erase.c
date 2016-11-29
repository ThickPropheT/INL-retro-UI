#include "erase.h"

int erase_nes( USBtransfer *transfer ) 
{
	
	debug("erasing");
					//dict	opcode	 		   addr     		data
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		0,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		1,		0);
	debug("reset");
	dictionary_call( transfer,	BUFFER,	RAW_BUFFER_RESET,		0,		0);
	debug("read status");
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		0,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		1,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		4,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		7,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		8,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		14,		0);

	debug("allocate");
	dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER0,		0x1000,		4);
	dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER1,		0x2104,		4);
	dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER2,		0x3508,		4);
	dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER3,		0x4A0C,		4);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		0,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		1,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		2,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		3,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		4,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		5,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		6,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		7,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		8,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		9,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		10,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		11,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		12,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		13,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		14,		0);
	dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		15,		0);

					//dict	opcode	 		   addr     		data
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		0,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		1,		0);
	//debug("reset");
	//dictionary_call( transfer,	BUFFER,	RAW_BUFFER_RESET,		0,		0);
	//debug("read status");
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		0,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		1,		0);
	//debug("allocate 0");
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER0,		0x1A00,		1);
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER1,		0x2A01,		1);
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER2,		0x3A02,		1);
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER3,		0x4A03,		1);
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER4,		0x5A04,		1);
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER5,		0x6A05,		1);
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER6,		0x7A06,		1);
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER7,		0x8A07,		1);
	//debug("read status");
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		0,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		1,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		2,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		3,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		4,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		5,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		6,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		7,		0);

	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER7,		0x8A07,		1);
	//debug("reset");
	//dictionary_call( transfer,	BUFFER,	RAW_BUFFER_RESET,		0,		0);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		7,		0);
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER7,		0x8A07,		1);
	//dictionary_call( transfer,	BUFFER,	RAW_BANK_STATUS,		7,		0);
	//dictionary_call( transfer,	BUFFER,	ALLOCATE_BUFFER7,		0x5A05,		1);

	
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

//program byte
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x5555,		0xAA);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x2AAA,		0x55);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x5555,		0xA0);
//	dictionary_call( transfer,	NES,	DISCRETE_EXP0_PRGROM_WR,	0x8000,		0x00);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);
//	dictionary_call( transfer,	NES,	NES_CPU_RD,			0x8000,		0);

	return 0;

}
