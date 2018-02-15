
-- create the module's table
local bnrom = {}

-- import required modules
local dict = require "scripts.app.dict"
local dump = require "scripts.app.dump"
local flash = require "scripts.app.flash"

-- file constants

-- local functions
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
end

--base is the actual NES CPU address, not the rom offset (ie $FFF0, not $7FF0)
local function wr_bank_table(base, entries)


--	--first select the last bank as cartridge should be erased (all 0xFF)
--	--go ahead and write the value to where it's supposed to be incase rom isn't erased
--	dict.nes("NES_CPU_WR", base+entries-1, entries-1)
--
--	--write bank table to selected bank
--	while( i < entries) do
--		wr_flash_byte(base+i, i)
--		i = i+1;
--	end
--	--now we can use that bank table to jump to any other bank
	
	--smarter solution is to simply count down so we can use just one loop

	local cur_bank = entries - 1  --16 minus 1 is 15 = 0x0F

	while( cur_bank >= 0 ) do
		--select bank to write to (last bank first)
		--use the bank table to make the switch
		dict.nes("NES_CPU_WR", base+cur_bank, cur_bank)

		--write bank table to selected bank
		local i = 0
		while( i < entries) do
			wr_flash_byte(base+i, i)
			i = i+1;
		end

		cur_bank = cur_bank-1
	end

end

--Cart should be in reset state upon calling this function 
--this function processes all user requests for this specific board/mapper
local function process( read, erase, program, verify, dumpfile, flashfile, verifyfile)

	local rv = nil
	local file 

--initialize device i/o for NES
	dict.io("IO_RESET")
	dict.io("NES_INIT")


--dump the cart to dumpfile
	if read then
		file = assert(io.open(dumpfile, "wb"))

		--TODO find bank table to avoid bus conflicts!
		--dump cart into file
		dump.dumptofile( file, 512, "BxROM", "PRGROM", true )

		--close file
		assert(file:close())
	end


--erase the cart
	if erase then

		print("\nerasing BxROM");

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
		--Lizard's bank table is at $FF94 so hard code that for now
		wr_bank_table(0xFF94, 16)

		--flash cart
		flash.write_file( file, 512, "BxROM", "PRGROM", true )
		--close file
		assert(file:close())

	end

--verify flashfile is on the cart
	if verify then
		--for now let's just dump the file and verify manually

		file = assert(io.open(verifyfile, "wb"))

		--dump cart into file
		dump.dumptofile( file, 512, "BxROM", "PRGROM", true )

		--close file
		assert(file:close())
	end

	dict.io("IO_RESET")
end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
bnrom.process = process

-- return the module's table
return bnrom
