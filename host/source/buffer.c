#include "buffer.h"

/* Desc:Reset buffers on device
 * Pre: 
 * Post:All buffers and raw sram unallocated
 * Rtn: SUCCESS if no errors
 */
int reset_buffers( USBtransfer *transfer ) 
{
	return dictionary_call( transfer,	DICT_BUFFER,	RAW_BUFFER_RESET,	NILL,	NILL,	
								USB_IN,		NULL,	1);
}

/* Desc:allocate buffers on device
 * Pre: buffers must be reset
 * Post:All buffers and raw sram unallocated
 *	Sets id, status to EMPTY, and last_idx.
 *	All other elements set to zero
 * Rtn: SUCCESS if no errors
 */
int allocate_buffers( USBtransfer *transfer, int num_buffers, int buff_size ) {

	//TODO verify number of banks doesn't exceed devices' configuration
//	uint8_t rv[RV_DATA0_IDX];
//	uint8_t rv;
	int rv;
	int buff0id = 0;
	int buff0basebank = 0;
	int buff1id = 0;
	int buff1basebank = 0;
	int numbanks= 0;

	//want to allocate buffers as makes sense based on num and size
	//Ideally a buffer will be 256Bytes which equals a page size
	//256Bytes doesn't work well with dumping though as max xfr size is 254Bytes
	//So for simplicity dumping starts with 128B buffers
	//But this means a single buffer can't hold a full page
	//In this case the missing bits between buffer size and page_num must be contained
	//in upper bits of the buffer id.
	if( (num_buffers == 2) && (buff_size == 128)) {
		//MSB tells buffer value of A7 when operating
		buff0id = 0x00;
		buff0basebank = 0;
		numbanks= buff_size/RAW_BANK_SIZE;
		buff1id = 0x80;
		buff1basebank= numbanks;	//buff1 starts right after buff0
	} else {
		sentinel("Not setup to handle this buffer config");
	}

	//allocate buffer0
	rv = dictionary_call_debug( transfer,	DICT_BUFFER,	ALLOCATE_BUFFER0,	
				( (buff0id<<8)|(buff0basebank) ),	numbanks,	
						USB_IN,		NULL,	1);
	if ( rv != SUCCESS ){
		//failed to allocate pass error code back
		return rv;
	}
	//allocate buffer1
	rv = dictionary_call_debug( transfer,	DICT_BUFFER,	ALLOCATE_BUFFER1,	
				( (buff1id<<8)|(buff1basebank) ),	numbanks,
						USB_IN,		NULL,	1);
	if ( rv != SUCCESS ){
		//failed to allocate pass error code back
		return rv;
	}

	return SUCCESS;
error:
	return ~SUCCESS;
}


