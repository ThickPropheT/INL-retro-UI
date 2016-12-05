#include "cartridge.h"

int detect_console( cartridge *cart, USBtransfer *transfer ) 
{
	printf("attempting to detect cartridge...\n");
	//always start with resetting i/o
	io_reset( transfer );

	//TODO check if can detect a cart inserted backwards before continuing

	//check if NES/Famicom cart 
	nes_init( transfer );

	//if PPU /A13 is tied to CIRAM /CE we know it's NES/Famicom
	if ( jumper_ciramce_ppuA13n( transfer ) ) {
		//NES with 2 screen mirroring
		debug("CIRAM /CE is jumpered to PPU /A13");
		cart->console = NES_CART;
	} else if ( ciramce_inv_ppuA13( transfer ) ) {
		//some boards including INLXO-ROM boards drive CIRAM /CE with inverse of PPU A13
		debug("CIRAM /CE is inverse of PPU A13");
		cart->console = NES_CART;
	} else {
		//TODO check if CIRAM on cartridge or NT CHR-ROM
	}

	//if NES/FC determine which if possible
	//also possible that this could catch failed detections above which is current case with VRC6
	//Famicom has audio in and audio out pins connected to each other
	//For this to pass with a NES cart EXP6 would have to be jumpered to EXP0 for some strange reason
	//might fail if expansion audio mixing circuitry foils the check above
	//but worst case we detected NES when famicom which isn't big deal..
	if ( famicom_sound( transfer ) ) {
		debug("Famicom audio jumper found");
		cart->console = FC_CART;
	}

	//if couldn't detect NES/FC check for SNES cartridge
	//want to keep from outputting on EXP bus if NES cart was found
	if ( cart->console == UNKNOWN ) {
		//only way I can think of is if memory is enabled by certain addresses and control signals
		snes_init( transfer );
		if ( snes_mem_visible( transfer, 0x00, 0xFFFC )) {
			//CHECK for memory visible near NES reset vector
			debug("SNES memories detected");
			cart->console = SNES_CART;
		}
		//now it's possible that rom is there, but data is 0xFF so above test would fail
		//one option would be to drive bus low for short period and see if bus can be
		//driven low.  This could damage pin drivers though, best to create command in 
		//firmware to perform this to limit to one CPU cycle instead of USB xfr times

		//Prob best to check if able to read flash ID's if reset vector data is 0xFF
		//Since reset vector being 0xFF prob means it's blank flash cart..

		//playable SNES carts should have data somewhere in reset vector...
	}

	//always end with resetting i/o
	io_reset( transfer );

	switch (cart->console) {
		case NES_CART: printf("NES cartridge detected!\n");	
			break;
		case FC_CART: printf("Famicom cartridge detected!\n");	
			break;
		case SNES_CART: printf("SNES cartridge detected!\n");	
			break;
		case BKWD_CART: log_err("CARTRIDGE INSERTED BACKWARDS!!!\n");	
			//TODO detection not yet implemented need to look over connector pinouts
			break;
		case UNKNOWN: printf("Unable to detect cartridge...\n");
			//TODO error out properly
			break;
		default:
			sentinel("cartridge console element got set to something unsupported.");
	}

	return SUCCESS;

error:
	return -1;

}

	//Can detect INL discrete, XO, and possily others with pullup EXP0 test
	//These carts have pullups on EXP0 so rising edge is faster
	//Also detecting that CIRAM /CE is driven by inverted PPU A13 is good way to 
	//detect INLX0-ROM boards.

	//SNES /RESET pin should control whether memory is enabled on original boards
	//INL SNES boards memory mapping is controlled by /RESET pin
	//roms are still visible when /RESET low, but SRAM isn't

