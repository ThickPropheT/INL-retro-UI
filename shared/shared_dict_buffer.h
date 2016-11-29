#ifndef _shared_dict_buffer_h
#define _shared_dict_buffer_h

//define dictionary's reference number in the shared_dictionaries.h file
//then include this dictionary file in shared_dictionaries.h
//The dictionary number is literally used as usb transfer request field
//the opcodes and operands in this dictionary are fed directly into usb setup packet's wValue wIndex fields


//=============================================================================================
//=============================================================================================
// BUFFER DICTIONARY
//
// opcodes contained in this dictionary must be implemented in firmware/source/buffer.c
//
//=============================================================================================
//=============================================================================================


//raw buffer banks & size
//This determines the raw buffer sram space on avr at firmware compile time
//one byte per bank is instantiated to keep track of that banks' allocation status
//a buffer must be as large as one bank, but a buffer can consume multiple banks
//only limit to buffer size is the buffer structure
//current buffer structure utilizes a single byte for current byte counter
//which limits to 256 bytes per buffer currently
//having 16bit value support would expand this, or somehow shifting current byte
//to account for multiple bytes could expand further
//#define NUM_RAW_BANKS   8	// 8*32 = 256 bytes of buffer
#define NUM_RAW_BANKS   16	//16*32 = 512 bytes of buffer

#define RAW_BANK_SIZE   32      //bank size in bytes

//number of buffer objects
//This controls number of static buffer objects instantiated in firmware at compile time
//note this also controls opcodes created/supported by firmware
//reducing number of buffers frees firmware sram by ~16bytes per buffer object
//not much reason to have less than 2 atleast allow double buffering
//so one can be getting loaded/unloaded by USB while other is dumping/flashing
//current max is 8, but only really limited by opcode definitions to address all buffers
//makes #ifdef code simpler to only allow buffer numbers that are power of 2
//#define NUM_BUFFERS_2	2
//#define NUM_BUFFERS_4	4
#define NUM_BUFFERS_8	8

//defined here so identical to host and firmware
//status values
#define EMPTY 		0x00
#define PROBLEM		0x10
#define USB_UNLOADING	0x80
#define USB_LOADING	0x90
#define USB_FULL	0x98
#define CHECKING	0xC0
#define DUMPING		0xD0
#define DUMP_FULL	0xD8
#define ERASING		0xE0
#define FLASHING	0xF0
#define FLASH_WAIT	0xF8
#define UNALLOC 	0xFF




//=============================================================================================
//	OPCODES with up to 24bit operand and no return value besides SUCCESS/ERROR_CODE
//=============================================================================================
//	Detect this opcode/operand setup with opcode between the following defines:
#define BUFF_OPCODE_NRV_MIN	0x00
#define BUFF_OPCODE_NRV_MAX	0x3F
//
#define BUFF_OPCODE_RV_MIN	0x40
#define BUFF_OPCODE_RV_MAX	0x7F
//=============================================================================================
//=============================================================================================

//blindly clear all allocation of raw buffer space
//reset all buffers to unallocated
#define	RAW_BUFFER_RESET	0x00

//send bank number and read back it's status
//0xFF-UNALLOC
//gets assigned buffer ID number when allocated
#define RAW_BANK_STATUS		0x40

//=============================================================================================
//	OPCODES with up to 24bit operand and no return value besides SUCCESS/ERROR_CODE
//	BUFFER NUMBER denoted in lower nibble of opcode
//=============================================================================================
//	Detect this opcode group which uses 3 LSbits to determine which buffer to call
#define BUFF_OPCODE_BUFN_MIN	0x80
#define BUFF_OPCODE_BUFN_MAX	0xFF
//
//	Detect this opcode/operand setup with opcode between the following defines:
#define BUFF_OPCODE_BUFN_NRV_MIN	0x80
#define BUFF_OPCODE_BUFN_NRV_MAX	0xBF
//
#define BUFF_OPCODE_BUFN_RV_MIN		0xC0
#define BUFF_OPCODE_BUFN_RV_MAX		0xEF
//
#define BUFF_OPCODE_PAYLOAD_MIN		0xF0
#define BUFF_OPCODE_PAYLOAD_MAX		0xFF
//=============================================================================================
//=============================================================================================
//allocate firmware sram to a buffer
//send a buffer number
//buffer size
//base address 0-255 (in 32byte chunks)
//returns SUCCESS if able to allocate
//returns error code if unable to allocate
#define	ALLOCATE_BUFFER0	0x80
#define	ALLOCATE_BUFFER1	0x81
#define	ALLOCATE_BUFFER2	0x82
#define	ALLOCATE_BUFFER3	0x83
#define	ALLOCATE_BUFFER4	0x84
#define	ALLOCATE_BUFFER5	0x85
#define	ALLOCATE_BUFFER6	0x86
#define	ALLOCATE_BUFFER7	0x87


//designate what buffer to fill with opcode
//endpoint direction determines if read/write
#define BUFF_PAYLOAD0		0xF0
#define BUFF_PAYLOAD1		0xF1
#define BUFF_PAYLOAD2		0xF2
#define BUFF_PAYLOAD3		0xF3
#define BUFF_PAYLOAD4		0xF4
#define BUFF_PAYLOAD5		0xF5
#define BUFF_PAYLOAD6		0xF6
#define BUFF_PAYLOAD7		0xF7



//~16 bytes per buffer...
//as initially defined in firmware
//typedef struct buffer{
//	uint8_t 	*data;		//pointer to base buffer's allocated sram
//	uint8_t 	size;		//size of buffer in bytes (max 256 bytes)
//	uint8_t		status;		//current status of buffer USB load/unload, flashing, waiting, erase
//	uint8_t 	cur_byte;	//byte currently being loaded/unloaded/flashed/read
//	uint8_t		reload;		//add this number to page_num for next loading
//	uint8_t 	id;		//address bits between buffer size and page number
//					//ie need 2x128 byte buffers making buff_num = A7
//					//ie need 4x64 byte buffers making buff_num = A7:6
//					//ie need 8x32 byte buffers making buff_num = A7:5
//					//CANNOT BE 0xFF "UNALLOC"
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

//buffer struct
//designed to have multiple buffers instantiated at a time
//buffers are effectively objects who get instructed by host
//commands include things like reading from cartridge rom/ram
//getting filled by usb writes, programming 'themselves' to
//where they belong on the cartridge.  They know how to get
//on/off the cart and have a current status that the host can
//query.  Host then decides when to refill/dump data from
//buffer over usb.  Host also can query buffer to get error
//codes back and and attempt to resolve issues.  Buffers can
//also be instructed to verify themselves with a checksum
//They can verify themselves against cart rom/ram as well.

//atmega164 has 1KByte of SRAM
//VUSB has max of 254 + wValue/wIndex = max 256 bytes without long transfers
//Expecting two 128 byte buffers will be faster than a single 256byte buffer
//That way one buffer could be flashing onto the cart while the other is 
//getting filled 8bytes at a time from USB.  this would proide some double
//buffering speed ups, but not sure how fast that'll be.  Having buffers
//as small as 32bytes might be faster for 4-8MB flash memories as they
//have ability to flash 32Bytes at once.  Could perhaps fill multiple
//32B buffers with one larger 128/256B transfer.  Increasing transfer size
//does speed up overall due to fewer setup and status USB packets.

//USB control transfer:
//	SETUP/DATA STAGES:
//	Token packet- sync, pid, addr, enp, crc5, eop = 35bits
//	Data/setup packet
//	sync, pid, crc16, eop = 67bits
//	setup/payload packet data = 64bits
//	handsk (sync, pid, eop) = 19bit
//	total = 185bits with 64bit payload = 34.6% data utilization per data packet
//	STATUS STAGE:
//	same as above, but zero lenght data packet.
//	total 121bits

//	8byte total payload
//	185 setup + 185 data + 121 status = 491 bits transferred for 64bit payload = 13.03% bus utilization

//	32byte total payload
//	185 setup + 4*185 data + 121 status = 1046 bits xfrd for 4*64=256 payld = 24.47% bus util

//	64byte total payload
//	185 setup + 8*185 data + 121 status = 1786 bits xfrd for 8*64=512 payld = 28.67% bus util

//	128byte total payload
//	185 setup + 16*185 data + 121 status = 3266 bits xfrd for 16*64=1024 payld = 31.35% bus util

//	254byte total payload
//	185 setup + 32*185-16 data + 121 status = 6210 bits xfrd for 31.8*64=2032 payld = 32.72% bus util
//	4.4% speedup than 128

//	256bytes in 254byte xfr 
//	185 setup + 32*185-16 data + 121 status = 6210 bits xfrd for 32*64=2048 payld = 32.98% bus util
//	0.79% speedup than 254 in 254

//	256byte total payload
//	185 setup + 32*185 data + 121 status = 6226 bits xfrd for 32*64=2048 payld = 32.89% bus util

//	512byte total payload
//	185 setup + 64*185 data + 121 status = 12146 bits xfrd for 64*64=4096 payld = 33.72% bus util
//	1% greater bus util compared to 254byte xfr
//	2.2% speedup compared to 256 in 254


//	Probably decent overall speedup by eliminating multiple status packets.  Not sure how many
//	NAK's the old firmware was sending while the device programmed the entire 256byte buffer.
//	but one NAK is more than should be necessary.
//
//	Plan is to support max of 254 byte transfers with 2 bytes stuffed in setup packet
//	Want to compare to 512B long transfers for speed comparison
//	
//	Either way, I can setup the buffers in smaller sizes than the transfers.  Then a buffer could
//	start programming mid usb transfer once it's full.  Want to make effor to hide flash programming
//	wait time behind usb transfer time.



#endif
