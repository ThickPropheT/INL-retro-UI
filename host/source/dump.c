#include "dump.h"

/* Desc:
 * Pre: 
 * Post:
 * Rtn: 
 */
//main collected as much data about cart as possible without reading roms
//now it's time to start running CRC's to try and finalize mapper/config
//Once final mapper is known store header data in rom file and start dumping!
int dump_cart( USBtransfer* transfer, rom_image *rom, cartridge *cart )
{
	debug("dumping cart");

	//TODO provide user arg to force all these checks passed
	//first check if any provided args differ from what was detected
	check( cart->console != UNKNOWN, 
		"cartridge not detected, must provide console if autodetection is off");

	if ( rom->console != UNKNOWN ) {
		check( rom->console == cart->console, 
			"request system dump doesn't match detected cartridge");
	}
	if ( (cart->mapper != UNKNOWN) && (rom->mapper != UNKNOWN) ) {
		check( rom->mapper == cart->mapper,	
			"request mapper dump doesn't match detected mapper");
	}

	//Run some CRC's to determine size of memories

	//setup buffers and manager
	//reset buffers first
	check(! reset_buffers( transfer ), "Unable to reset device buffers");
	//need to allocate some buffers for dumping
	//2x 128Byte buffers
	int num_buffers = 2;
	int buff_size = 128;	
	check(! allocate_buffers( transfer, num_buffers, buff_size ), "Unable to allocate buffers");

	//set buffer elements as needed
	//set reload which gets added to page_num after each buffer read
	//set page_num to non-zero if offset arg sent
	//set mem_type and part_num to designate how to get/write data
	//set multiple and add_mult only when flashing
	//set mapper, map_var, and function to designate read/write algo

	//tell buffers what function to use for dumping

	//inform buffer manager to start dumping operation now that buffers are initialized

	//manager updates buffer status' so they'll start dumping
	//once they're full manager prepares them to be read back on USB payloads
	//once the next payload request happens manager knows last buffer can start dumping again
	//buffer updates it's elements and goes off to dump next page
	//tell buffer manager when to stop
	//reset buffers and setup to dump CHR-ROM

	return SUCCESS;
error:
	return ~SUCCESS;
}
