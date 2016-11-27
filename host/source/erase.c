#include "erase.h"

int erase_nes( USBtransfer *transfer ) 
{
	
	debug("erasing");

					//dict		opcode	 		   addr     		data
	dictionary_call( transfer,	IO,		IO_RESET,			0,		0);
	dictionary_call( transfer,	IO,		NES_INIT,			0,		0);
	dictionary_call( transfer,	NES,		DISCRETE_EXP0_PRGROM_WR,	0x5555,		0xAA);
	dictionary_call( transfer,	NES,		DISCRETE_EXP0_PRGROM_WR,	0x2AAA,		0x55);
	dictionary_call( transfer,	NES,		DISCRETE_EXP0_PRGROM_WR,	0x5555,		0x90);
	dictionary_call( transfer,	NES,		EMULATE_NES_CPU_RD,		0x8000,		0);
	dictionary_call( transfer,	NES,		EMULATE_NES_CPU_RD,		0x8001,		0);

	return 0;

}
