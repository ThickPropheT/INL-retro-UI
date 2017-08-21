#include "dump.h"

/* Desc:Dump cart memory into buffer's data array
 * Pre: buffer elements must be updated to designate how/where to dump
 * 	buffer's cur_byte must be cleared or set to where to start dumping
 * Post:page dumped from cart memory to buffer.
 * Rtn: SUCCESS or ERROR# depending on if there were errors.
 */
uint8_t dump_buff( buffer *buff ) {

	uint8_t addrH = buff->page_num;	//A15:8  while accessing page
//warn	uint8_t addrX;	//A23:16 while accessing page

	//TODO use mapper to set mapper controlled address bits

	//use mem_type to set addrH/X as needed for dump loop
	//also use to get read function pointer
	switch ( buff->mem_type ) {
		case PRGROM:
			addrH |= 0x80;	//$8000
			buff->cur_byte = nes_cpu_page_rd_poll( buff->data, addrH, buff->id, 
								//id contains MSb of page when <256B buffer
								buff->last_idx, ~FALSE );
			break;
		case CHRROM:		//$0000
			buff->cur_byte = nes_ppu_page_rd_poll( buff->data, addrH, buff->id,
								buff->last_idx, ~FALSE );
			break;
		case PRGRAM:
			addrH |= 0x60;	//$6000
			buff->cur_byte = nes_cpu_page_rd_poll( buff->data, addrH, buff->id,
								buff->last_idx, ~FALSE );
			break;
		case SNESROM:
			addrH |= 0x80;	//$8000 LOROM space
			//need to split page_num
			//A14-8 page_num[7-0]
			//A15 high (LOROM)
			//A23-16 page_num[14-8]
			HADDR_SET( (buff->page_num)>>7 );
			buff->cur_byte = snes_rom_page_rd_poll( buff->data, addrH, buff->id, 
								//id contains MSb of page when <256B buffer
								buff->last_idx, ~FALSE );
		case SNESRAM:
//warn			addrX = ((buff->page_num)>>8);
			break;
		default:
			return ERR_BUFF_UNSUP_MEM_TYPE;
	}

	return SUCCESS;
}
