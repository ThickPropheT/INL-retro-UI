#ifndef _buffer_h
#define _buffer_h

#include <avr/io.h>


//~16 bytes per buffer...
typedef struct buffer {
	uint8_t 	*data;		//pointer to base buffer's allocated sram
	uint8_t 	size;		//size of buffer in bytes (max 256 bytes)
	uint8_t		status;		//current status of buffer USB load/unload, flashing, waiting, erase
	uint8_t 	cur_byte;	//byte currently being loaded/unloaded/flashed/read
	uint8_t		reload;		//add this number to page_num for next loading
	uint8_t 	id;		//address bits between buffer size and page number
					//ie need 2x128 byte buffers making buff_num = A7
					//ie need 4x64 byte buffers making buff_num = A7:6
					//ie need 8x32 byte buffers making buff_num = A7:5
	uint16_t 	page_num;	//address bits beyond buffer's size and buff_num A23-A8
					//MSB A23-16, LSB A15-8
	uint8_t		mem_type;	//SNES ROM, SNES RAM, PRG ROM, PRG RAM, CHR ROM, CHR RAM, CPLD, SPI
	uint8_t		part_num;	//used to define unlock commands, sector erase, etc
	uint8_t		multiple;	//number of times to program this page
	uint8_t		add_mult;	//add this number to page_num for multiple programs
					//CHR shift LSb to A13 (max 2MByte)
					//PRG shift LSb to A14 (max 4MByte)
					//SNES add to MSB of page_num (max 16MByte)
	uint8_t		mapper;		//mapper number of board
	uint8_t		mapvar;		//mapper variant 
	uint8_t		function;	//function "pointer" for flash/dump operation control
}buffer;

static buffer buff0;
static buffer buff1;
static buffer buff2;
static buffer buff3;
static buffer buff4;
static buffer buff5;
static buffer buff6;
static buffer buff7;

#include "logic.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"

uint8_t buffer_opcode_no_return( uint8_t opcode, buffer *buff, uint8_t oper1, uint8_t oper2, uint8_t oper3 );
void raw_buffer_reset( );
uint8_t allocate_buffer( buffer *buff, uint8_t new_id, uint8_t base_bank, uint8_t num_banks );



#endif
