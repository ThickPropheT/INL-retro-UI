#include "flash.h"


uint8_t	write_page( uint8_t bank, uint8_t addrH, buffer *buff, write_funcptr wr_func, read_funcptr rd_func )
{
	uint16_t cur = buff->cur_byte;
	uint8_t	n = buff->cur_byte;
	uint8_t read;
//	extern operation_info *oper_info;

	while ( cur <= buff->last_idx ) {
		//write unlock sequence
		//need to make address and unlock data variable
		//best for host to communcate these values
		//actual value is part mapper dependent and part flash dependent
		//mapper controlled address bits dictate where split is
		//32KB banking A14-0 NES ctl, A15+ mapper ctl "bank" NROM, BNROM, ANROM
		//addrH_dmask   = 0b0111 1111 directly addressable addrH bits
		//page2bankshft = A14->A8 = 7 shifts (equal to number of set bits in addrH_mask
		//16KB banking A13-0 NES ctl, A14+ mapper ctl "bank" UxROM, MMC1
		//addrH_dmask   = 0b0011 1111
		//page2bankshft = A13->A8 = 6 shifts
		// 8KB banking A12-0 NES ctl, A13+ mapper ctl "bank" MMC3, FME7
		//addrH_dmask   = 0b0001 1111
		//page2bankshft = A12->A8 = 5 shifts
		// 4KB banking A11-0 NES ctl, A12+ mapper ctl "bank" ezNSF
		//addrH_dmask   = 0b0000 1111
		//page2bankshft = A11->A8 = 4 shifts
		wr_func( 0x5555, 0xAA );
//		wr_func( oper_info->unlock1_AH, oper_info->unlock1_AL, oper_info->unlock1_data );
		wr_func( 0x2AAA, 0x55 );
//		wr_func( oper_info->unlock2_AH, oper_info->unlock2_AL, oper_info->unlock2_data );
		wr_func( 0x5555, 0xA0 );
//		wr_func( oper_info->command_AH, oper_info->command_AL, oper_info->command1_data );
		wr_func( ((addrH<<8)| n), buff->data[n] );
	
		do {
			usbPoll();
			read = rd_func((addrH<<8)|n);
	
		} while( read != rd_func((addrH<<8)|n) );
		//TODO verify byte is value that was trying to be flashed
		//move on to next byte
		n++;
		cur++;

	}

	buff->cur_byte = n;

	return SUCCESS;

} 

uint8_t	write_page_chr( uint8_t bank, uint8_t addrH, buffer *buff, write_funcptr wr_func, read_funcptr rd_func )
{
	uint16_t cur = buff->cur_byte;
	uint8_t	n = buff->cur_byte;
	uint8_t read;
//	extern operation_info *oper_info;

	while ( cur <= buff->last_idx ) {
		//write unlock sequence
		//need to make address and unlock data variable
		//best for host to communcate these values
		//actual value is part mapper dependent and part flash dependent
		//mapper controlled address bits dictate where split is
		//32KB banking A14-0 NES ctl, A15+ mapper ctl "bank" NROM, BNROM, ANROM
		//addrH_dmask   = 0b0111 1111 directly addressable addrH bits
		//page2bankshft = A14->A8 = 7 shifts (equal to number of set bits in addrH_mask
		//16KB banking A13-0 NES ctl, A14+ mapper ctl "bank" UxROM, MMC1
		//addrH_dmask   = 0b0011 1111
		//page2bankshft = A13->A8 = 6 shifts
		// 8KB banking A12-0 NES ctl, A13+ mapper ctl "bank" MMC3, FME7
		//addrH_dmask   = 0b0001 1111
		//page2bankshft = A12->A8 = 5 shifts
		// 4KB banking A11-0 NES ctl, A12+ mapper ctl "bank" ezNSF
		//addrH_dmask   = 0b0000 1111
		//page2bankshft = A11->A8 = 4 shifts
		wr_func( 0x1555, 0xAA );
//		wr_func( oper_info->unlock1_AH, oper_info->unlock1_AL, oper_info->unlock1_data );
		wr_func( 0x0AAA, 0x55 );
//		wr_func( oper_info->unlock2_AH, oper_info->unlock2_AL, oper_info->unlock2_data );
		wr_func( 0x1555, 0xA0 );
//		wr_func( oper_info->command_AH, oper_info->command_AL, oper_info->command1_data );
		wr_func( ((addrH<<8)| n), buff->data[n] );
		//wr_func( ((addrH<<8)| n), buff->page_num );
		//wr_func( ((addrH<<8)| n),  addrH);
	
		do {
			usbPoll();
			read = rd_func((addrH<<8)|n);
	
		} while( read != rd_func((addrH<<8)|n) );
		//TODO verify byte is value that was trying to be flashed
		//move on to next byte
		n++;
		cur++;

	}

	buff->cur_byte = n;

	return SUCCESS;

} 

uint8_t	write_page_snes( uint8_t bank, uint8_t addrH, buffer *buff, write_funcptr wr_func, read_funcptr rd_func )
{
	uint16_t cur = buff->cur_byte;
	uint8_t	n = buff->cur_byte;
	uint8_t read;
//	extern operation_info *oper_info;
//	uint8_t cur_data = buff->data[n];

#ifdef AVR_CORE
		wdt_reset();	
#endif


	//set to program mode for first entry
	EXP0_LO();

	//enter unlock bypass mode
	wr_func( 0x0AAA, 0xAA );
	wr_func( 0x0555, 0x55 );
	wr_func( 0x0AAA, 0x20 );


	while ( cur <= buff->last_idx ) {
		//write unlock sequence
		//need to make address and unlock data variable
		//best for host to communcate these values
		//actual value is part mapper dependent and part flash dependent
		//mapper controlled address bits dictate where split is
		//32KB banking A14-0 NES ctl, A15+ mapper ctl "bank" NROM, BNROM, ANROM
		//addrH_dmask   = 0b0111 1111 directly addressable addrH bits
		//page2bankshft = A14->A8 = 7 shifts (equal to number of set bits in addrH_mask
		//16KB banking A13-0 NES ctl, A14+ mapper ctl "bank" UxROM, MMC1
		//addrH_dmask   = 0b0011 1111
		//page2bankshft = A13->A8 = 6 shifts
		// 8KB banking A12-0 NES ctl, A13+ mapper ctl "bank" MMC3, FME7
		//addrH_dmask   = 0b0001 1111
		//page2bankshft = A12->A8 = 5 shifts
		// 4KB banking A11-0 NES ctl, A12+ mapper ctl "bank" ezNSF
		//addrH_dmask   = 0b0000 1111
		//page2bankshft = A11->A8 = 4 shifts
		//

		//unlocked	wr_func( 0x0AAA, 0xAA );
		//unlocked	wr_func( 0x0555, 0x55 );

		//wr_func( 0x0000, 0xA0 );
		snes_rom_wr_cur_addr( 0xA0 );	//gained ~3KBps (59.13KBps) inl6 with v3.0 proto

		wr_func( ((addrH<<8)| n), buff->data[n] );
		//wr_func( ((addrH<<8)| n), cur_data );  //didn't actually speed up

		//Targetting 2MByte 16mbit flash which doesn't have buffered writes
		//currently have average flash speed of 21.05KBps going to start removing some of these NOPs
		//and optimizing flash routine to get time down.
	
		//exit program mode
		EXP0_HI();
		//pre-fetch next byte of data
		//cur_data = buff->data[n+1];
#ifdef AVR_CORE
		wdt_reset();	
#endif

		//wait for byte to flash
	//	do {
	//		usbPoll();
	//		read = rd_func((addrH<<8)|n);
	//
	//	//} while( read != rd_func((addrH<<8)|n) );
	//	} while( read != buff->data[n] );
		//this can cause things to hang on failed programs..
		//need a smarter flash polling algo, kind of a pain because we don't have
		//a good way to toggle /OE or /CE quickly on v3 SNES boards


		usbPoll();
		read = rd_func((addrH<<8)|n);
		//prepare for upcoming write cycle, or allow for a polling read
		EXP0_LO();
		//First check if already outputting final data
		if (read != buff->data[n] )  {
			//if not, lets see if toggle is occuring
			EXP0_HI();
			while( read != rd_func((addrH<<8)|n) ){
				EXP0_LO();
				NOP(); NOP(); NOP(); NOP();
				NOP(); NOP(); NOP(); NOP();
				NOP(); NOP(); NOP(); NOP();
				EXP0_HI();
				read = rd_func((addrH<<8)|n);
			}
			//prepare for upcoming write cycle
			EXP0_LO();
		}

//		//IDK why, but AVR will exit early sometimes
//		//without this second check, ~20 errors per 32KByte on SNES v3.0
//		//All error bytes are 0xFF instead of true data
//		//may need a smarter flash polling routine..
//		//Tried to add extra delay to read algo, and didn't change anything
//		//Also have decent trust in read routine as it's comparable to page read
//		//which works flawlessly for dumps.  So think it has to do with flashing specifically...
//		//Hmm maybe the avr is missing a read..  flash /CE, /OE, and /WE never toggle
//		//so why would flash polling output different data between polls..?
//		//Ahh this is the issue, adding the code below only adds delay which gives flash
//		//enough time to complete write.


		//retry if write failed
		//this helped but still seeing similar fails to dumps
	//	if (read == buff->data[n]) {
			n++;
			cur++;
	//	}

	}

	buff->cur_byte = n;

	//exit unlock bypass mode
	wr_func( 0x0000, 0x90 );
	wr_func( 0x0000, 0x00 );
	//reset the flash chip, supposed to exit too
	wr_func( 0x0000, 0xF0 );

	//exit program mode
	EXP0_HI();

	return SUCCESS;

} 

/* Desc:Flash buffer contents on to cartridge memory
 * Pre: buffer elements must be updated to designate how/where to flash
 * 	buffer's cur_byte must be cleared or set to where to start flashing
 *	mapper registers must be initialized
 * Post:buffer page flashed/programmed to memory.
 * Rtn: SUCCESS or ERROR# depending on if there were errors.
 */
uint8_t flash_buff( buffer *buff ) {

	uint8_t addrH = buff->page_num;	//A15:8  while accessing page

	//First need to initialize mapper register bits
	//Perhaps this only needs to be done on first buffer though..?
	//Actually think this is best handled from buffer.c in operation == STARTFLASH

	//TODO use mapper to set mapper controlled address bits

	//need to calculate current bank and addrH

	//TODO set unlock addresses based on what works for that mapper and how it's banks are initialized

	//use mem_type to set addrH/X as needed for dump loop
	//also use to get read function pointer
	switch ( buff->mem_type ) {
		case PRGROM:		//$8000
			write_page( 0, (0x80 | addrH), buff, discrete_exp0_prgrom_wr, nes_cpu_rd );
			break;
		case CHRROM:		//$0000
			write_page_chr( 0, addrH, buff, nes_ppu_wr, nes_ppu_rd );
			break;
		case PRGRAM:
			//addrH |= 0x60;	//$6000
			//buff->cur_byte = nes_cpu_page_wr_poll( buff->data, addrH, buff->id,
		//						buff->last_idx, ~FALSE );
			break;
		case SNESROM:
			addrH |= 0x80;	//$8000 LOROM space
			//need to split page_num
			//A14-8 page_num[7-0]
			//A15 high (LOROM)
			//A23-16 page_num[14-8]
			HADDR_SET( (buff->page_num)>>7 );
			write_page_snes( 0, addrH, buff, snes_rom_wr, snes_rom_rd );
		case SNESRAM:
//warn			addrX = ((buff->page_num)>>8);
			break;
		default:
			return ERR_BUFF_UNSUP_MEM_TYPE;
	}


	//lets start just reading first page of PRG-ROM then get fancy
//	while (buff->cur_byte < buff->last_idx) {
//
//		//might be faster to put some of these in new pointers, but not sure..
//		buff->data[buff->cur_byte] = nes_cpu_rd( addrH, buff->cur_byte );
//		buff->cur_byte++;
//	}

	return SUCCESS;
}
