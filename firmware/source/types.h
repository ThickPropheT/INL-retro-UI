#ifndef _types_h
#define _types_h

typedef struct setup_packet{
	uint8_t		bmRequestType;	//contains endpoint
	uint8_t		bRequest;	//designates dictionary of opcode
	uint8_t		opcode;		//wValueLSB (little endian)
	uint8_t		miscdata;	//wValueMSB 
	uint8_t		operandLSB;	//wIndexLSB
	uint8_t		operandMSB;	//wIndexMSB
	uint16_t	wLength;
}setup_packet;

//~16 bytes per buffer...
typedef struct buffer {
	uint8_t 	*data;		//pointer to base buffer's allocated sram
	//uint8_t 	size;		//size of buffer in bytes (max 256 bytes) THIS DOESN'T work 256B = 9bit value
	uint8_t 	last_idx;	//index of last byte in buffer used to determine when at end of buffer
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

#endif
