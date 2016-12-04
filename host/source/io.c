#include "io.h"

/* Desc:disable all possible outputs and float EXP0
 * Pre: 
 * Post:all memories disabled data bus clear
 * Rtn: 
 */
void io_reset( USBtransfer *transfer ) 
{
	dictionary_call( transfer,	IO,	IO_RESET,		0,	0,	
					USB_IN,		NULL,	1);
}

/* Desc:initialize NES I/P and O/P
 * Pre: 
 * Post:memories disabled data bus clear
 * Rtn: 
 */
void nes_init( USBtransfer *transfer ) 
{
	dictionary_call( transfer,	IO,	NES_INIT,		0,	0,	
					USB_IN,		NULL,	1);
}


/* Desc:initialize SNES I/P and O/P
 * Pre: 
 * Post:memories disabled data bus clear
 * Rtn: 
 */
void snes_init( USBtransfer *transfer ) 
{
	dictionary_call( transfer,	IO,	SNES_INIT,		0,	0,	
					USB_IN,		NULL,	1);
}


