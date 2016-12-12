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
 *	sets reload to sum of buffers
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

		//allocate buffer0
		rv = dictionary_call( transfer,	DICT_BUFFER,	ALLOCATE_BUFFER0,	
					( (buff0id<<8)|(buff0basebank) ),	numbanks,	
							USB_IN,		NULL,	1);
		if ( rv != SUCCESS ){
			//failed to allocate pass error code back
			return rv;
		}
		//allocate buffer1
		rv = dictionary_call( transfer,	DICT_BUFFER,	ALLOCATE_BUFFER1,	
					( (buff1id<<8)|(buff1basebank) ),	numbanks,
							USB_IN,		NULL,	1);
		if ( rv != SUCCESS ){
			//failed to allocate pass error code back
			return rv;
		}

		//set reload (value added to page_num after each load/dump to sum of buffers
		// 2 * 128 = 256 -> reload = 1
		//set buffer0
		dictionary_call( transfer,	DICT_BUFFER,	SET_RELOAD_PAGENUM0,	0x0000,	0x01,
							USB_IN,		NULL,	1);
		//set buffer1
		dictionary_call( transfer,	DICT_BUFFER,	SET_RELOAD_PAGENUM1,	0x0000,	0x01,
							USB_IN,		NULL,	1);
		

	} else {
		sentinel("Not setup to handle this buffer config");
	}


	return SUCCESS;
error:
	return ~SUCCESS;
}


/* Desc:Set buffer mem_type and part_num
 * Pre: buffer is allocated
 * Post:both value set to buffer number passed in
 * Rtn: SUCCESS if no errors
 */
int set_mem_n_part( USBtransfer *transfer, int buff_num, int mem_type, int part_num ) 
{
	return dictionary_call( transfer,	DICT_BUFFER,	SET_MEM_N_PART,	
				//( (mem_type<<8) | (part_num) ),		buff_num,	USB_IN,	NULL,	1);
				( (mem_type<<8) | (part_num) ),		1,	USB_IN,	NULL,	1);
				// 0xAABB,		buff_num,	USB_IN,	NULL,	1);
}

/* Desc:Set buffer mapper and map_var
 * Pre: buffer is allocated
 * Post:both value set to buffer number passed in
 * Rtn: SUCCESS if no errors
 */
int set_map_n_mapvar( USBtransfer *transfer, int buff_num, int mapper, int map_var ) 
{
	return dictionary_call( transfer,	DICT_BUFFER,	SET_MAP_N_MAPVAR,	
				( (mapper<<8) | (map_var) ),		buff_num,	USB_IN,	NULL,	1);
				// 0xCCDD,		buff_num,	USB_IN,	NULL,	1);
}

/* Desc:Set buffer manager operation 
 * Pre: buffers are allocated and elements set ready to start operation
 * Post:operation starts on device
 * Rtn: SUCCESS if no errors
 */
int set_buff_operation( USBtransfer *transfer, int operation ) 
{
	return dictionary_call( transfer,	DICT_BUFFER,	SET_BUFFER_OPERATION,	operation, 	
					NILL,	USB_IN,	NULL,	1);
}

/* Desc:Payload IN transfer
 * Pre: buffers are allocated operation started
 * Post:payload of length stored in data
 * Rtn: SUCCESS if no errors
 */
int payload_in( USBtransfer *transfer, uint8_t *data, int length ) 
{
	return dictionary_call( transfer,	DICT_BUFFER,	BUFF_PAYLOAD,	NILL, 	NILL,	
							USB_IN,		data,	length);
}

/* Desc:Get buffer elements and print them
 * Pre: buffers are allocated
 * Post:
 * Rtn: SUCCESS if no errors
 */
int get_buff_elements( USBtransfer *transfer, int buff_num ) 
{
	printf("pri buff%d:", buff_num);
	dictionary_call_debug( transfer,	DICT_BUFFER,	GET_PRI_ELEMENTS,	NILL, 	buff_num,
								USB_IN,		NULL,	8);
	printf("sec buff%d:", buff_num);
	dictionary_call_debug( transfer,	DICT_BUFFER,	GET_SEC_ELEMENTS,	NILL, 	buff_num,
								USB_IN,		NULL,	8);
	return SUCCESS;
}

/* Desc:Get buffer opertationt
 * Pre: 
 * Post:
 * Rtn: SUCCESS if no errors
 */
int get_buff_operation( USBtransfer *transfer ) 
{
	printf("operation:");
	dictionary_call_debug( transfer,	DICT_BUFFER,	GET_BUFF_OPERATION,	NILL, 	NILL,
								USB_IN,		NULL,	RV_DATA0_IDX+1);
	return SUCCESS;
}
