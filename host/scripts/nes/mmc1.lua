
-- create the module's table
local mmc1 = {}

-- import required modules
local dict = require "scripts.app.dict"
local nes = require "scripts.app.nes"
local dump = require "scripts.app.dump"
local flash = require "scripts.app.flash"

-- file constants

-- local functions

local function init_mapper( debug )

	--MMC1 ignores all but the first write
	dict.nes("NES_CPU_RD", 0x8000)
	--reset MMC1 shift register with D7 set
	dict.nes("NES_CPU_WR", 0x8000, 0x80)

--	mmc1_write(0x8000, 0x10);       //32KB mode, prg bank @ $8000-FFFF, 4KB CHR mode
	dict.nes("NES_MMC1_WR", 0x8000, 0x10)
--	//note the mapper will constantly reset to this when writing to PRG-ROM
--	//PRG-ROM A18-A14

--	mmc1_write(0xE000, 0x00);       //16KB bank @ $8000
	dict.nes("NES_MMC1_WR", 0xE000, 0x00)
--	//CHR-ROM A16-12 (A14-12 are required to be valid)

--	mmc1_write(0xA000, 0x02);       //4KB bank @ PT0  $2AAA cmd and writes
	dict.nes("NES_MMC1_WR", 0xA000, 0x02)
--	mmc1_write(0xC000, 0x05);       //4KB bank @ PT1  $5555 cmd fixed
	dict.nes("NES_MMC1_WR", 0xC000, 0x05)
end


--test the mapper's mirroring modes to verify working properly
--can be used to help identify board: returns true if pass, false if failed
local function mirror_test( debug )

	--put MMC1 in known state (mirror bits cleared)
	init_mapper() 

	--MM = 0: 1 screen A
	dict.nes("NES_MMC1_WR", 0x8000, 0x00)
	if (nes.detect_mapper_mirroring() ~= "1SCNA") then
		print("MMC1 mirror test fail")
		return false
	end

	--MM = 1: 1 screen B
	dict.nes("NES_MMC1_WR", 0x8000, 0x01)
	if (nes.detect_mapper_mirroring() ~= "1SCNB") then
		print("MMC1 mirror test fail")
		return false
	end

	--MM = 2: Vertical
	dict.nes("NES_MMC1_WR", 0x8000, 0x02)
	if (nes.detect_mapper_mirroring() ~= "VERT") then
		print("MMC1 mirror test fail")
		return false
	end

	--MM = 3: Horizontal
	dict.nes("NES_MMC1_WR", 0x8000, 0x03)
	if (nes.detect_mapper_mirroring() ~= "HORZ") then
		print("MMC1 mirror test fail")
		return false
	end

	--passed all tests
	if(debug) then print("MMC1 mirror test passed") end
	return true
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
end


--read PRG-ROM flash ID
local function prgrom_manf_id( debug )

	init_mapper()

	if debug then print("reading PRG-ROM manf ID") end
	--A0-A14 are all directly addressable in CNROM mode
	--and mapper writes don't affect PRG banking
	dict.nes("NES_CPU_WR", 0xD555, 0xAA)
	dict.nes("NES_CPU_WR", 0xAAAA, 0x55)
	dict.nes("NES_CPU_WR", 0xD555, 0x90)
	rv = dict.nes("NES_CPU_RD", 0x8000)
	if debug then print("attempted read PRG-ROM manf ID:", string.format("%X", rv)) end
	rv = dict.nes("NES_CPU_RD", 0x8001)
	if debug then print("attempted read PRG-ROM prod ID:", string.format("%X", rv)) end

	--exit software
	dict.nes("NES_CPU_WR", 0x8000, 0xF0)

end

--read CHR-ROM flash ID
local function chrrom_manf_id( debug )

	init_mapper()

	if debug then print("reading CHR-ROM manf ID") end
	--A0-A14 are all directly addressable in CNROM mode
	--and mapper writes don't affect PRG banking
	dict.nes("NES_PPU_WR", 0x1555, 0xAA)
	dict.nes("NES_PPU_WR", 0x0AAA, 0x55)
	dict.nes("NES_PPU_WR", 0x1555, 0x90)
	rv = dict.nes("NES_PPU_RD", 0x0000)
	if debug then print("attempted read CHR-ROM manf ID:", string.format("%X", rv)) end
	rv = dict.nes("NES_PPU_RD", 0x0001)
	if debug then print("attempted read CHR-ROM prod ID:", string.format("%X", rv)) end

	--exit software
	dict.nes("NES_PPU_WR", 0x8000, 0xF0)

end




--Cart should be in reset state upon calling this function 
--this function processes all user requests for this specific board/mapper
local function process( test, read, erase, program, verify, dumpfile, flashfile, verifyfile)

	local rv = nil
	local file 

--initialize device i/o for NES
	dict.io("IO_RESET")
	dict.io("NES_INIT")

--test cart by reading manf/prod ID
	if test then

		--verify mirroring is behaving as expected
		mirror_test(true)

		nes.ppu_ram_sense(0x1000, true)
		print("EXP0 pull-up test:", dict.io("EXP0_PULLUP_TEST"))	

		--attempt to read PRG-ROM flash ID
		prgrom_manf_id(true)
		--attempt to read CHR-ROM flash ID
		chrrom_manf_id(true)
	end

--dump the cart to dumpfile
	if read then
		init_mapper()	--32KB PRG-ROM banks

		file = assert(io.open(dumpfile, "wb"))

		--dump cart into file
		dump.dumptofile( file, 256, "MMC1", "PRGROM", true )
		dump.dumptofile( file, 128, "MMC1", "CHRROM", true )

		--close file
		assert(file:close())
	end


--erase the cart
	if erase then

		init_mapper()

		print("\nerasing MMC1");

		print("erasing PRG-ROM");
		dict.nes("NES_CPU_WR", 0xD555, 0xAA)
		dict.nes("NES_CPU_WR", 0xAAAA, 0x55)
		dict.nes("NES_CPU_WR", 0xD555, 0x80)
		dict.nes("NES_CPU_WR", 0xD555, 0xAA)
		dict.nes("NES_CPU_WR", 0xAAAA, 0x55)
		dict.nes("NES_CPU_WR", 0xD555, 0x10)
		rv = dict.nes("NES_CPU_RD", 0x8000)

		local i = 0

		--TODO create some function to pass the read value 
		--that's smart enough to figure out if the board is actually erasing or not
		while ( rv ~= 0xFF ) do
			rv = dict.nes("NES_CPU_RD", 0x8000)
			i = i + 1
		end
		print(i, "naks, done erasing prg.");


		--TODO erase CHR-ROM only if present
		init_mapper()

		print("erasing CHR-ROM");
		dict.nes("NES_PPU_WR", 0x1555, 0xAA)
		dict.nes("NES_PPU_WR", 0x0AAA, 0x55)
		dict.nes("NES_PPU_WR", 0x1555, 0x80)
		dict.nes("NES_PPU_WR", 0x1555, 0xAA)
		dict.nes("NES_PPU_WR", 0x0AAA, 0x55)
		dict.nes("NES_PPU_WR", 0x1555, 0x10)
		rv = dict.nes("NES_PPU_RD", 0x8000)

		local i = 0

		--TODO create some function to pass the read value 
		--that's smart enough to figure out if the board is actually erasing or not
		while ( rv ~= 0xFF ) do
			rv = dict.nes("NES_PPU_RD", 0x8000)
			i = i + 1
		end
		print(i, "naks, done erasing chr.");


	end


--program flashfile to the cart
	if program then

		--open file
		file = assert(io.open(flashfile, "rb"))
		--determine if auto-doubling, deinterleaving, etc, 
		--needs done to make board compatible with rom

		--flash cart
		flash.write_file( file, 256, "MMC1", "PRGROM", true )
		flash.write_file( file, 128, "MMC1", "CHRROM", true )
		--close file
		assert(file:close())

	end

--verify flashfile is on the cart
	if verify then
		--for now let's just dump the file and verify manually

		file = assert(io.open(verifyfile, "wb"))

		--dump cart into file
		dump.dumptofile( file, 256, "MMC1", "PRGROM", true )
		dump.dumptofile( file, 128, "MMC1", "CHRROM", true )

		--close file
		assert(file:close())
	end

	dict.io("IO_RESET")
end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
mmc1.process = process

-- return the module's table
return mmc1
