#include "dump.h"




/* Desc:Dump cart memory into buffer's data array
 * Pre: buffer elements must be updated to designate how/where to dump
 * 	buffer's cur_byte must be cleared or set to where to start dumping
 * Post:page dumped from cart memory to buffer.
 * Rtn: SUCCESS or ERROR# depending on if there were errors.
 */
uint8_t dump_page( buffer *buff ) {

	uint8_t addrH = (buff->page_num | 0x80);	//or in $8000 to set equiv CPU address

	//lets start just reading first page of PRG-ROM then get fancy
	while (buff->cur_byte < buff->last_idx) {

		//might be faster to put some of these in new pointers, but not sure..
		buff->data[buff->cur_byte] = nes_cpu_rd( addrH, buff->cur_byte );
		buff->cur_byte++;
	}

	return SUCCESS;
}
