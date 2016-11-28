#include "buffer.h"

//typedef struct buffer{
//	uint8_t 	*data;		//pointer to base buffer's allocated sram
//	uint8_t 	size;		//size of buffer in bytes (max 256 bytes)
//	uint8_t		status;		//current status of buffer USB load/unload, flashing, waiting, erase
//	uint8_t 	cur_byte;	//byte currently being loaded/unloaded/flashed/read
//	uint8_t		reload;		//add this number to page_num for next loading
//	uint8_t 	buff_num;	//address bits between buffer size and page number
//					//ie need 2x128 byte buffers making buff_num = A7
//					//ie need 4x64 byte buffers making buff_num = A7:6
//					//ie need 8x32 byte buffers making buff_num = A7:5
//	uint16_t 	page_num;	//address bits beyond buffer's size and buff_num A23-A8
//					//MSB A23-16, LSB A15-8
//	uint8_t		mem_type;	//SNES ROM, SNES RAM, PRG ROM, PRG RAM, CHR ROM, CHR RAM, CPLD, SPI
//	uint8_t		part_num;	//used to define unlock commands, sector erase, etc
//	uint8_t		multiple;	//number of times to program this page
//	uint8_t		add_mult;	//add this number to page_num for multiple programs
//					//CHR shift LSb to A13 (max 2MByte)
//					//PRG shift LSb to A14 (max 4MByte)
//					//SNES add to MSB of page_num (max 16MByte)
//	uint8_t		mapper;		//mapper number of board
//	uint8_t		mapvar;		//mapper variant 
//	uint8_t		function;	//function "pointer" for flash/dump operation control
//}buffer;

//max raw buffer size is 256 bytes must create multiple raw buffers for more
//or handle 16bit values for page size

//raw buffer memory to which smaller buffers will be created from
//set pointers and lengths to prevent buffer conflicts
//static uint8_t raw_buffer[NUM_RAW_BANKS * RAW_BANK_SIZE];	//8 banks of 32bytes each 256Bytes total
static uint8_t raw_buffer[256];	//8 banks of 32bytes each 256Bytes total

//buffer status stores allocation status of each raw buffer 32Byte bank
//static uint8_t raw_bank_status[NUM_RAW_BANKS]; 
static uint8_t raw_bank_status[8]; 


/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated function.
 * 	shared_dict_buffer.h is used in both host and fw to ensure opcodes/names align
 * Pre: Macros must be defined in firmware pinport.h
 * 	opcode must be defined in shared_dict_buffer.h
 * Post:function call complete.
 * Rtn: SUCCESS if opcode found, ERR_UNKN_BUFF_OPCODE_NRV if opcode not present.
 */
uint8_t buffer_opcode_no_return( uint8_t opcode, buffer *buff, uint8_t oper1, uint8_t oper2, uint8_t oper3 )
{
	switch (opcode) { 
		case RAW_BUFFER_RESET:	
			raw_buffer_reset();	
			break;
		case ALLOCATE_BUFFER0 ... ALLOCATE_BUFFER7:	
			allocate_buffer( &(*buff), oper1, oper2, oper3 );
			break;
		default:
			 //opcode doesn't exist
			 return ERR_UNKN_BUFF_OPCODE_NRV;
	}
	
	return SUCCESS;

}

/* Desc:Blindly resets all buffer allocation and values
 * 	Host instructs this to be called.
 * Pre: static instantitions of raw_buffer, raw_bank_status, and buff0-7 above
 * Post:all raw buffer ram unallocated
 * 	buffer status updated to UNALLOC
 * Rtn:	None
 */
void raw_buffer_reset( )
{
	uint8_t i;

	//unallocate raw buffer space
	for( i=0; i<NUM_RAW_BANKS; i++) {
		raw_bank_status[i] = UNALLOC;
	}

	//unallocate all buffer objects
	buff0.status = UNALLOC;
	buff1.status = UNALLOC;
	buff2.status = UNALLOC;
	buff3.status = UNALLOC;
	buff4.status = UNALLOC;
	buff5.status = UNALLOC;
	buff6.status = UNALLOC;
	buff7.status = UNALLOC;
	//set buffer id to UNALLOC
	buff0.id = UNALLOC;
	buff1.id = UNALLOC;
	buff2.id = UNALLOC;
	buff3.id = UNALLOC;
	buff4.id = UNALLOC;
	buff5.id = UNALLOC;
	buff6.id = UNALLOC;
	buff7.id = UNALLOC;

}

/* Desc:Embeded subtitute for malloc of a buffer object
 * 	Host instructs this to be called so the host
 * 	is in charge of what buffers are for what
 * 	and how things are used.  This function does
 * 	keep track of each bank of the raw buffer.
 * 	It will not allocate buffer space and return error
 * 	if host is trying to allocate buffer on top of 
 * 	another buffer or bank already allocated.
 * 	pass in pointer to buffer object to be allocated
 * 	pass base bank number and number of banks in buffer
 * Pre: static instantitions of raw_buffer raw_bank_status,
 * 	and buff0-7 above.
 * 	Buffer must be unallocated.
 * 	new id cannot be 0xFF/255 "UNALLOC"
 * 	bank allocation request can't go beyond raw ram space
 * Post:section of raw buffer allocated for host use
 * 	status of raw buffer updated to prevent future collisions
 * 	bank status byte contains buffer's id
 * 	buffer status updated from UNALLOC to EMPTY
 * 	all other buffer values cleared to zero
 * Rtn:	SUCCESS or ERROR code if unable to allocate
 */
//uint8_t allocate_buffer( struct buffer *buff, uint8_t new_id, uint8_t base_bank, uint8_t num_banks )
uint8_t allocate_buffer( buffer *buff, uint8_t new_id, uint8_t base_bank, uint8_t num_banks )
{
	uint8_t i;

	//check incoming args
	if ( (base_bank+num_banks) > NUM_RAW_BANKS ) {
		//trying to allocate SRAM past end of raw_buffer
		return ERR_BUFF_ALLOC_RANGE;
	}

	//check that buffer isn't already allocated
	if ( buff->status != UNALLOC) {
		return ERR_BUFF_STATUS_ALREADY_ALLOC;
	}
	if ( buff->id != UNALLOC) {
		return ERR_BUFF_ID_ALREADY_ALLOC;
	}

	//check that raw banks aren't allocated
	for ( i=0; i<num_banks; i++) {
		if ( raw_bank_status[base_bank+i] != UNALLOC ) {
			return ERR_BUFF_RAW_ALREADY_ALLOC;
		}
	}

	//seems that buffer and raw are free allocate them as requested
	buff->id = new_id;
	buff->status = EMPTY;
	buff->data = &raw_buffer[base_bank];

	//set bank status to bank's id
	for ( i=0; i<num_banks; i++) {
		raw_bank_status[base_bank+i] = new_id;
	}

	return SUCCESS;	

}





