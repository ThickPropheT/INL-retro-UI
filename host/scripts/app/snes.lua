
-- create the module's table
local snes = {}

-- import required modules
local dict = require "scripts.app.dict"

-- file constants
local RESET_VECT_HI = 0xFFFD
local RESET_VECT_LO = 0xFFFC

-- local functions

-- Desc:read reset vector from passed in bank
-- Pre: snes_init() been called to setup i/o
-- Post:Address left on bus memories disabled
-- Rtn: reset vector that was found
local function read_reset_vector( bank, debug )

	--ensure cart is in play mode
	dict.pinport("CTL_SET_HI", "SNES_RST")

	--first set SNES bank A16-23
	dict.snes("SNES_SET_BANK", bank)

	--read reset vector high byte
	vector = dict.snes("SNES_ROM_RD", RESET_VECT_HI)
	--shift high byte of vector to where it belongs
	vector = vector << 8
	--read low byte of vector
	vector = vector | dict.snes("SNES_ROM_RD", RESET_VECT_LO)

	if debug then print("SNES bank:", bank, "reset vector", string.format("$%x", vector) ) end

	return vector
end

-- Desc: attempt to read flash rom ID 
-- Pre: snes_init() been called to setup i/o
-- Post:Address left on bus memories disabled
-- Rtn: true if flash ID found
local function read_flashID( debug )

	local rv
	--enter software mode A11 is highest address bit that needs to be valid
	--datasheet not exactly explicit, A11 might not need to be valid
	--part has A-1 (negative 1) since it's in byte mode, meaning the part's A11 is actually A12
	--WR $AAA:AA $555:55 $AAA:AA
	dict.snes("SNES_SET_BANK", 0x00)

	--put cart in program mode
	dict.pinport("CTL_SET_LO", "SNES_RST")

	dict.snes("SNES_ROM_WR", 0x0AAA, 0xAA)
	dict.snes("SNES_ROM_WR", 0x0555, 0x55)
	dict.snes("SNES_ROM_WR", 0x0AAA, 0x90)
	--read manf ID
	rv = dict.snes("SNES_ROM_RD", 0x0000)
	if debug then print("attempted read SNES ROM manf ID:", string.format("%X", rv)) end
--	if ( rv[RV_DATA0_IDX] != SST_MANF_ID ) {
--		return GEN_FAIL;
--		//no need for software exit since failed to enter
--	}
--
	--read prod ID
	rv = dict.snes("SNES_ROM_RD", 0x0002)
	if debug then print("attempted read SNES ROM prod ID:", string.format("%X", rv)) end
	rv = dict.snes("SNES_ROM_RD", 0x001C)
	if debug then print("attempted read SNES density ID: ", string.format("%X", rv)) end
	rv = dict.snes("SNES_ROM_RD", 0x001E)
	if debug then print("attempted read SNES boot sect ID:", string.format("%X", rv)) end
--	if ( (rv[RV_DATA0_IDX] == SST_PROD_128)
--	||   (rv[RV_DATA0_IDX] == SST_PROD_256)
--	||   (rv[RV_DATA0_IDX] == SST_PROD_512) ) {
--		//found expected manf and prod ID
--		flash->manf = SST_MANF_ID;
--		flash->part = rv[RV_DATA0_IDX];
--		flash->wr_dict = DICT_NES;
--		flash->wr_opcode = NES_PPU_WR;
--	}
--
	--exit software
--	dict.nes("NES_PPU_WR", 0x0000, 0xF0)
	dict.snes("SNES_ROM_WR", 0x0000, 0xF0)
--
	
	--exit program mode
	dict.pinport("CTL_SET_HI", "SNES_RST")

	--return true

end

-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
snes.read_reset_vector = read_reset_vector
snes.read_flashID = read_flashID

-- return the module's table
return snes

