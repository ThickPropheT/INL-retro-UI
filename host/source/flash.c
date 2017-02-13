#include "flash.h"

/* Desc:
 * Pre: 
 * Post:
 * Rtn: 
 */
//should know what mapper the board is by the time we get here
//might be hard to tell if all flash is addressable until start writing data though
int flash_cart( USBtransfer* transfer, rom_image *rom, cartridge *cart )
{
	//make some checks to ensure rom is compatible with cart

	//first do some checks like ensuring proper areas or sectors are blank

	//erase sectors or chip as needed

	//reset, allocate, and initialize device buffers

	//initialize mapper registers as needed for memory being programmed

	//set device operation to STARTFLASH

	//send payload data

	//run checksums to verify successful flash operation

	int num_buffers = 2;
	int buff_size = 256;	
	int buff0 = 0;
	int buff1 = 1;
	int i;
	uint8_t data[buff_size];


	debug("flashing cart");

	//TODO provide user arg to force all these checks passed
	//first check if any provided args differ from what was detected
	check( (cart->console != UNKNOWN), "cartridge not detected, must provide console if autodetection is off");

	if ( rom->console != UNKNOWN ) {
		check( rom->console == cart->console, 
			"request system dump doesn't match detected cartridge");
	}
	if ( (cart->mapper != UNKNOWN) && (rom->mapper != UNKNOWN) ) {
		check( rom->mapper == cart->mapper,	
			"request mapper dump doesn't match detected mapper");
	}

	//start with reset and init
	io_reset( transfer );
	nes_init( transfer );
	//Run some CRC's to determine size of memories
	
	//start operation at reset	
	check(! set_operation( transfer, RESET ), "Unable to set buffer operation");

	//setup buffers and manager
	//reset buffers first
	check(! reset_buffers( transfer ), "Unable to reset device buffers");
	//need to allocate some buffers for flashing
	//2x 256Byte buffers
	check(! allocate_buffers( transfer, num_buffers, buff_size ), "Unable to allocate buffers");

	//tell buffers what function to use for flashing
	//load operation elements into buff0 and then copy buff0 to oper_info
	load_oper_info_elements( transfer, cart );
	get_oper_info_elements( transfer );

	//setup buffers and manager
	//reset buffers first
	check(! reset_buffers( transfer ), "Unable to reset device buffers");
	//need to allocate some buffers for flashing
	//2x 256Byte buffers
	check(! allocate_buffers( transfer, num_buffers, buff_size ), "Unable to allocate buffers");

	//set mem_type and part_num to designate how to get/write data
	check(! set_mem_n_part( transfer, buff0, PRGROM, SST_MANF_ID ), "Unable to set mem_type and part");
	check(! set_mem_n_part( transfer, buff1, PRGROM, SST_MANF_ID ), "Unable to set mem_type and part");
	//set multiple and add_mult only when flashing
	//TODO
	//set mapper, map_var, and function to designate read/write algo

	//just dump visible NROM memory to start
	check(! set_map_n_mapvar( transfer, buff0, NROM, NILL ), "Unable to set mapper and map_var");
	check(! set_map_n_mapvar( transfer, buff1, NROM, NILL ), "Unable to set mapper and map_var");

	//debugging print out buffer elements
	get_operation( transfer );
	get_buff_elements( transfer, buff0 );
	get_buff_elements( transfer, buff1 );

	debug("\n\nsetting operation STARTFLASH");
	//inform buffer manager to start dumping operation now that buffers are initialized
	check(! set_operation( transfer, STARTFLASH ), "Unable to set buffer operation");

//	//manager updates buffer status' so they'll start dumping
//	//once they're full manager prepares them to be read back on USB payloads
//	//once the next payload request happens manager knows last buffer can start dumping again
//	//buffer updates it's elements and goes off to dump next page
//
//	debug("first payload");
//	check(! read_from_file( rom, data, buff_size ), "Error with file read");
//	//check(! payload_out( transfer, data, buff_size ), "Error with payload OUT");
//	payload_out( transfer, data, buff_size );
//	get_operation( transfer );
//	get_buff_elements( transfer, buff0 );
//	get_buff_elements( transfer, buff1 );
//
//	debug("first payload done");
////	get_operation( transfer );
////	get_buff_elements( transfer, buff0 );
////	get_buff_elements( transfer, buff1 );
////
//	debug("second payload");
//	check(! read_from_file( rom, data, buff_size ), "Error with file read");
//	check(! payload_out( transfer, data, buff_size ), "Error with payload OUT");

//	get_operation( transfer );
//	get_buff_elements( transfer, buff0 );
//	get_buff_elements( transfer, buff1 );

	clock_t tstart, tstop;
	tstart = clock();

	//now just need to call series of payload IN transfers to retrieve data
	
	for( i=0; i<(32*KByte/buff_size); i++) {
	//for( i=0; i<8; i++) {
	//debug("\n\npayload out #%d", i);
	//get_operation( transfer );
	get_buff_elements( transfer, buff0 );
	get_buff_elements( transfer, buff1 );
		check(! read_from_file( rom, data, buff_size ), "Error with file read");
		check(! payload_out( transfer, data, buff_size ), "Error with payload OUT");
		//if ( i % 256 == 0 ) debug("payload in #%d", i);
		if ( i % 32 == 0 ) debug("payload out #%d", i);
	}
	get_operation( transfer );
	get_buff_elements( transfer, buff0 );
	get_buff_elements( transfer, buff1 );
	get_buff_elements( transfer, buff0 );
	get_buff_elements( transfer, buff1 );
	get_buff_elements( transfer, buff0 );
	get_buff_elements( transfer, buff1 );
	debug("payload done");

	//end operation at reset	
	check(! set_operation( transfer, RESET ), "Unable to set buffer operation");


/*
	//for( i=0; i<(512*KByte/buff_size); i++) {
	for( i=0; i<(32*KByte/buff_size); i++) {
	//for( i=0; i<(8*KByte/buff_size); i++) {
		check(! read_from_file( rom, data, buff_size ), "Error with file read");
		check(! payload_out( transfer, data, buff_size ), "Error with payload OUT");
		//if ( i % 256 == 0 ) debug("payload in #%d", i);
		if ( i % 32 == 0 ) debug("payload out #%d", i);
	}
	debug("payload done");
	//need to delay further USB commands until last buffer is flashed
	//TODO spin while buff1 status == FLASHING
	//currently this takes about as long as two get elements
	get_buff_elements( transfer, buff1 );
	get_buff_elements( transfer, buff1 );
	get_buff_elements( transfer, buff1 );

	tstop = clock();
	float timediff = ( (float)(tstop-tstart) / CLOCKS_PER_SEC);
	printf("total time: %fsec, speed: %fKBps\n", timediff, (512/timediff));
	//TODO flush file from time to time..?


	//tell buffer manager when to stop
	// or not..?  just reset buffers and start next memory or quit
	//reset buffers and setup to dump CHR-ROM

	check(! reset_buffers( transfer ), "Unable to reset device buffers");
	check(! allocate_buffers( transfer, num_buffers, buff_size ), "Unable to allocate buffers");
	check(! set_mem_n_part( transfer, buff0, CHRROM, SST_MANF_ID ), "Unable to set mem_type and part");
	check(! set_mem_n_part( transfer, buff1, CHRROM, SST_MANF_ID ), "Unable to set mem_type and part");
	check(! set_map_n_mapvar( transfer, buff0, NROM, NILL ), "Unable to set mapper and map_var");
	check(! set_map_n_mapvar( transfer, buff1, NROM, NILL ), "Unable to set mapper and map_var");

	debug("\n\nsetting operation STARTFLASH");
	//inform buffer manager to start dumping operation now that buffers are initialized
	check(! set_operation( transfer, STARTFLASH ), "Unable to set buffer operation");

	for( i=0; i<(8*KByte/buff_size); i++) {
		check(! read_from_file( rom, data, buff_size ), "Error with file read");
		check(! payload_out( transfer, data, buff_size ), "Error with payload OUT");
		//if ( i % 256 == 0 ) debug("payload in #%d", i);
		if ( i % 32 == 0 ) debug("payload out #%d", i);
	}
	debug("payload done");
*/

	//close file in main

	//reset io at end
	io_reset( transfer );

	return SUCCESS;
error:
	return ~SUCCESS;
}
