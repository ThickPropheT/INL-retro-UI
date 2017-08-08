
-- create the module's table
local erase = {}

-- import required modules
local dict = require "scripts.app.dict"

-- file constants

-- local functions
local function erase_nes()

	local rv = nil

	print("erasing_nrom");

	dict.io("IO_RESET")
	dict.io("NES_INIT")
	
	print("erasing PRG-ROM");
	dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0xAA)
	dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x2AAA, 0x55)
	dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0x80)
	dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0xAA)
	dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x2AAA, 0x55)
	dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0x10)
	rv = dict.nes("NES_CPU_RD", 0x8000)

	local i = 0

	while ( rv ~= 0xFF ) do
		rv = dict.nes("NES_CPU_RD", 0x8000)
		i = i + 1
	end
	print(i, " done erasing prg.\n");

	print("erasing CHR-ROM");
	dict.nes("NES_PPU_WR", 0x1555, 0xAA)
	dict.nes("NES_PPU_WR", 0x0AAA, 0x55)
	dict.nes("NES_PPU_WR", 0x1555, 0x80)
	dict.nes("NES_PPU_WR", 0x1555, 0xAA)
	dict.nes("NES_PPU_WR", 0x0AAA, 0x55)
	dict.nes("NES_PPU_WR", 0x1555, 0x10)
	rv = dict.nes("NES_PPU_RD", 0x0000)

	local i = 0

	while ( rv ~= 0xFF ) do
		rv = dict.nes("NES_PPU_RD", 0x0000)
		i = i + 1
	end
	print(i, " done erasing chr.\n");

end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
erase.erase_nes = erase_nes

-- return the module's table
return erase
