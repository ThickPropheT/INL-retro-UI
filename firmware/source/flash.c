#include "flash.h"


/* Desc:Programs buffer's data onto cart memory
 * Pre: Sector/Chip must be erased if required
 * 	buffer elements must be updated to designate how to program
 * Post:page flashed/programmed to designated memory.
 * Rtn: SUCCESS or ERROR# depending on if there were errors.
 */
uint8_t flash_page( buffer *buff ) {

	uint8_t addrH = (buff->page_num | 0x80);	//or in $8000 to set equiv CPU address
	uint8_t read;

	//lets start just reading first page of PRG-ROM then get fancy
	while (buff->cur_byte < buff->last_idx) {

		//write unlock sequence first
		discrete_exp0_prgrom_wr( 0x55, 0x55, 0xAA );
		discrete_exp0_prgrom_wr( 0x2A, 0xAA, 0x55 );
		discrete_exp0_prgrom_wr( 0x55, 0x55, 0xA0 );

		//then flash byte
		discrete_exp0_prgrom_wr( addrH, buff->cur_byte, buff->data[buff->cur_byte] );

		//then spin until write finished
		read =  nes_cpu_rd(addrH,buff->cur_byte);
		while ( read != nes_cpu_rd(addrH,buff->cur_byte) ) {
			read =  nes_cpu_rd(addrH,buff->cur_byte);
		}

		//byte stable, now verify proper value
		if ( read == buff->data[buff->cur_byte] ) {
			buff->cur_byte++;
			_LED_OFF();
		} else {//don't increment, retry
			_LED_OP();
			_LED_ON();
		}
	}

	return SUCCESS;


}



