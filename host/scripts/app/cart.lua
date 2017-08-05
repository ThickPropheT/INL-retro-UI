
-- create the module's table
local cart = {}

-- import required modules
local dict = require "scripts.app.dict"
local nes = require "scripts.app.nes"

-- file constants

-- local functions
local function detect( debug )

	print("attempting to detect cartridge...");
--	//always start with resetting i/o
	dict.io("IO_RESET")	

--	//TODO check if can detect a cart inserted backwards before continuing

--	//check if NES/Famicom cart 
	dict.io("NES_INIT")	

--	//if PPU /A13 is tied to CIRAM /CE we know it's NES/Famicom
	if nes.jumper_ciramce_ppuA13n(debug) then
--		//NES with 2 screen mirroring
		if debug then print("CIRAM /CE is jumpered to PPU /A13") end
--		cart->console = NES_CART;
	elseif nes.ciramce_inv_ppuA13(debug) then
--		//some boards including INLXO-ROM boards drive CIRAM /CE with inverse of PPU A13
		if debug then print("CIRAM /CE is inverse of PPU A13") end
--		cart->console = NES_CART;
	end
--	TODO check if CIRAM on cartridge or NT CHR-ROM
--
--	if NES/FC determine which if possible
--	also possible that this could catch failed detections above which is current case with VRC6
--	Famicom has audio in and audio out pins connected to each other
--	For this to pass with a NES cart EXP6 would have to be jumpered to EXP0 for some strange reason
--	might fail if expansion audio mixing circuitry foils the check above
--	but worst case we detected NES when famicom which isn't big deal..
	if nes.jumper_famicom_sound(debug) then
		if debug then print("Famicom audio jumper found") end
--		cart->console = FC_CART;
	end

--	//if couldn't detect NES/FC check for SNES cartridge
--	//want to keep from outputting on EXP bus if NES cart was found
--	if ( cart->console == UNKNOWN ) {
--		//only way I can think of is if memory is enabled by certain addresses and control signals
--		snes_init( transfer );
--		if ( snes_mem_visible( transfer, 0x00, 0xFFFC )) {
--			//CHECK for memory visible near NES reset vector
--			debug("SNES memories detected");
--			cart->console = SNES_CART;
--		}
--		//now it's possible that rom is there, but data is 0xFF so above test would fail
--		//one option would be to drive bus low for short period and see if bus can be
--		//driven low.  This could damage pin drivers though, best to create command in 
--		//firmware to perform this to limit to one CPU cycle instead of USB xfr times
--
--		//Prob best to check if able to read flash ID's if reset vector data is 0xFF
--		//Since reset vector being 0xFF prob means it's blank flash cart..
--
--		//playable SNES carts should have data somewhere in reset vector...
--	}
--
--	//always end with resetting i/o
--	io_reset( transfer );
--
--	switch (cart->console) {
--		case NES_CART: printf("NES cartridge detected!\n");	
--			break;
--		case FC_CART: printf("Famicom cartridge detected!\n");	
--			break;
--		case SNES_CART: printf("SNES cartridge detected!\n");	
--			break;
--		case BKWD_CART: log_err("CARTRIDGE INSERTED BACKWARDS!!!\n");	
--			//TODO detection not yet implemented need to look over connector pinouts
--			break;
--		case UNKNOWN: printf("Unable to detect cartridge...\n");
--			//TODO error out properly
--			break;
--		default:
--			sentinel("cartridge console element got set to something unsupported.");
--	}
--
--	return SUCCESS;
--
--error:
--	//always end with resetting i/o
--	io_reset( transfer );
--	return -1;
--
--}

end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
cart.detect = detect

-- return the module's table
return cart
