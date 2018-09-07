
-- create the module's table
local unrom = {}

-- import required modules
local dict = require "scripts.app.dict"
local nes = require "scripts.app.nes"
local dump = require "scripts.app.dump"
local flash = require "scripts.app.flash"

-- file constants

-- local functions
local function init_mapper( debug )
	--need to select bank0 so PRG-ROM A14 is low when writting to lower bank
	--TODO this needs to be written to rom where value is 0x00 due to bus conflicts
	--so need to find the bank table first!
	--this could present an even larger problem with a blank flash chip
	--would have to get a byte written to 0x00 first before able to change the bank..
	--becomes catch 22 situation.  Will have to rely on mcu over powering PRG-ROM..
	--ahh but a way out would be to disable the PRG-ROM with exp0 (/WE) going low
	--for now the write below seems to be working fine though..
	dict.nes("NES_CPU_WR", 0x8000, 0x00)
end

local function wr_flash_byte(addr, value, debug)

	dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0xAA)
	dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x2AAA, 0x55)
	dict.nes("DISCRETE_EXP0_PRGROM_WR", 0x5555, 0xA0)
	dict.nes("DISCRETE_EXP0_PRGROM_WR", addr, value)

	local rv = dict.nes("NES_CPU_RD", addr)

	local i = 0

	while ( rv ~= value ) do
		rv = dict.nes("NES_CPU_RD", addr)
		i = i + 1
	end
	if debug then print(i, "naks, done writing byte.") end

	--TODO report error if write failed
	
end

--base is the actual NES CPU address, not the rom offset (ie $FFF0, not $7FF0)
local function wr_bank_table(base, entries, numtables)

	local cur_bank 

	--need to have A14 clear when lower bank enabled
	init_mapper()

	--UxROM can have a single bank table in $C000-FFFF (assuming this is most likely)
	--or a bank table in all other banks in $8000-BFFF
	
	local i = 0
	while( i < entries) do
		wr_flash_byte(base+i, i)
		i = i+1;
	end

	--[[
	if( base >= 0xC000 ) then 
		--only need one bank table in last bank
		cur_bank = entries - 1  --16 minus 1 is 15 = 0x0F
	else
		--need bank table in all banks except last
		cur_bank = entries - 2  --16 minus 2 is 14 = 0x0E
	end


	while( cur_bank >= 0 ) do
		--select bank to write to (last bank first)
		--use the bank table to make the switch
		dict.nes("NES_CPU_WR", base+cur_bank, cur_bank)

		--write bank table to selected bank
		local i = 0
		while( i < entries) do
			print("write entry", i, "bank:", cur_bank)
			wr_flash_byte(base+i, i)
			i = i+1;
		end

		cur_bank = cur_bank-1

		if( base >= 0xC000 ) then 
			--only need one bank table in last bank
			break
		end
	end
	--]]

	--TODO verify the bank table was successfully written before continuing!

end

--Cart should be in reset state upon calling this function 
--this function processes all user requests for this specific board/mapper
local function process( test, read, erase, program, verify, dumpfile, flashfile, verifyfile)

	local rv = nil
	local file 
	local size = 128

--initialize device i/o for NES
	dict.io("IO_RESET")
	dict.io("NES_INIT")

--test cart by reading manf/prod ID
	if test then
		nes.detect_mapper_mirroring(true)
		nes.ppu_ram_sense(0x1000, true)
		print("EXP0 pull-up test:", dict.io("EXP0_PULLUP_TEST"))	

		--need to set PRG-ROM A14 low when lower bank selected
		init_mapper() 	--this may not succeed due to bus conflicts...
		nes.read_flashID_prgrom_exp0(true)
	end

--dump the cart to dumpfile
	if read then
		file = assert(io.open(dumpfile, "wb"))

		--TODO find bank table to avoid bus conflicts!
		--dump cart into file
		dump.dumptofile( file, size, "UxROM", "PRGROM", true )

		--close file
		assert(file:close())
	end


--erase the cart
	if erase then

		init_mapper()

		print("\nerasing UxROM");

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

	end


--program flashfile to the cart
	if program then

		--open file
		file = assert(io.open(flashfile, "rb"))
		--determine if auto-doubling, deinterleaving, etc, 
		--needs done to make board compatible with rom

		--find bank table in the rom
		--write bank table to all banks of cartridge
		--Nomolos' bank table is at $CC84 so hard code that for now
		--wr_bank_table(0xCC84, 32)
		--Owlia bank table
		--wr_bank_table(0xE473, 32)
		--rushnattack
		--wr_bank_table(0x8000, 8)
		--twindragons
		--wr_bank_table(0xC000, 32)
		--Armed for Battle
		wr_bank_table(0xFD69, 8)

		--flash cart
		flash.write_file( file, size, "UxROM", "PRGROM", true )
		--close file
		assert(file:close())

	end

--verify flashfile is on the cart
	if verify then
		--for now let's just dump the file and verify manually

		file = assert(io.open(verifyfile, "wb"))

		--dump cart into file
		dump.dumptofile( file, size, "UxROM", "PRGROM", true )

		--close file
		assert(file:close())
	end

	dict.io("IO_RESET")
end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
unrom.process = process

-- return the module's table
return unrom
