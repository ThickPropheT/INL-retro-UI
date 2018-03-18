
-- create the module's table
local nrom = {}

-- import required modules
local dict = require "scripts.app.dict"
local nes = require "scripts.app.nes"
local dump = require "scripts.app.dump"
local flash = require "scripts.app.flash"

-- file constants

-- local functions
--read PRG-ROM flash ID
local function prgrom_manf_id( debug )


end


--Cart should be in reset state upon calling this function 
--this function processes all user requests for this specific board/mapper
local function process( test, read, erase, program, verify, dumpfile, flashfile, verifyfile)

	local rv = nil
	local file 

--initialize device i/o for NES
	dict.io("IO_RESET")
	dict.io("NES_INIT")

--test the cart
	if test then
		nes.detect_mapper_mirroring(true)
		nes.read_flashID_chrrom_8K(true)
		print("EXP0 pull-up test:", dict.io("EXP0_PULLUP_TEST"))	
		nes.read_flashID_prgrom_exp0(true)
	end

--dump the cart to dumpfile
	if read then
		file = assert(io.open(dumpfile, "wb"))

		--dump cart into file
		dump.dumptofile( file, 32, "NROM", "PRGROM", true )
		dump.dumptofile( file, 8, "NROM", "CHRROM", true )

		--close file
		assert(file:close())
	end


--erase the cart
	if erase then

		print("erasing NROM");

		print("erasing PRG-ROM");
		dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0xAA)
		dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x2AAA, 0x55)
		dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0x80)
		dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0xAA)
		dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x2AAA, 0x55)
		dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0x10)
		rv = dict.nes("NES_CPU_RD", 0x8000)

		local i = 0

		--TODO create some function to pass the read value 
		--that's smart enough to figure out if the board is actually erasing or not
		while ( rv ~= 0xFF ) do
			rv = dict.nes("NES_CPU_RD", 0x8000)
			i = i + 1
		end
		print(i, "naks, done erasing prg.");

		print("erasing CHR-ROM");
		dict.nes("NES_PPU_WR", 0x1555, 0xAA)
		dict.nes("NES_PPU_WR", 0x0AAA, 0x55)
		dict.nes("NES_PPU_WR", 0x1555, 0x80)
		dict.nes("NES_PPU_WR", 0x1555, 0xAA)
		dict.nes("NES_PPU_WR", 0x0AAA, 0x55)
		dict.nes("NES_PPU_WR", 0x1555, 0x10)
		rv = dict.nes("NES_PPU_RD", 0x0000)

		while ( rv ~= 0xFF ) do
			rv = dict.nes("NES_PPU_RD", 0x0000)
			i = i + 1
		end
		print(i, "naks, done erasing chr.\n");
	end


--program flashfile to the cart
	if program then
		--open file
		file = assert(io.open(flashfile, "rb"))
		--determine if auto-doubling, deinterleaving, etc, 
		--needs done to make board compatible with rom
		--flash cart
		flash.write_file( file, 32, "NROM", "PRGROM", true )
		flash.write_file( file, 8, "NROM", "CHRROM", true )
		--close file
		assert(file:close())

	end

--verify flashfile is on the cart
	if verify then
		--for now let's just dump the file and verify manually

		file = assert(io.open(verifyfile, "wb"))

		--dump cart into file
		dump.dumptofile( file, 32, "NROM", "PRGROM", true )
		dump.dumptofile( file, 8, "NROM", "CHRROM", true )

		--close file
		assert(file:close())
	end

	dict.io("IO_RESET")
end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
nrom.process = process

-- return the module's table
return nrom
