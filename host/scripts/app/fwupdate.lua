
-- create the module's table
local fwupdate = {}

-- import required modules
local dict = require "scripts.app.dict"

-- file constants

-- local functions
local function erase_main()

	--dict.fwupdate("ERASE_1KB_PAGE", 2)	--page 0 & 1 (first 2KByte) are forbidden
	--dict.fwupdate("ERASE_1KB_PAGE", 3)	--this is redundant for RB (aligns C6 to RB when done with above)
	--dict.fwupdate("ERASE_1KB_PAGE", 4)	--0x0800_1000 - 0x0800_17FF
	--dict.fwupdate("ERASE_1KB_PAGE", 5)	--redundant RB
	--dict.fwupdate("ERASE_1KB_PAGE", 6)	--0x0800_1800 - 0x0800_1FFF
	--dict.fwupdate("ERASE_1KB_PAGE", 7)
	--dict.fwupdate("ERASE_1KB_PAGE", 8)	--0x0800_2000 - 0x0800_27FF
	--dict.fwupdate("ERASE_1KB_PAGE", 9)
	
	curpage = 2	--skip the first pages
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )

	while (curpage<32) do
--	while (curpage<128) do
		print("erasing page:", curpage)
		dict.fwupdate("ERASE_1KB_PAGE", curpage)

		rv = dict.fwupdate("GET_FLASH_ADDR") 
		print("flash addr:", string.format("%X", rv) )

		curpage = curpage+1
	end

end

local function update_firmware(newbuild)

	print("updating")

	--enter fwupdate mode
	dict.bootload("PREP_FWUPDATE")	

	--now the device will only respond to FWUPDATE dictionary commands
	
	--open new file
	file = assert(io.open(newbuild, "rb"))

	--TODO verify first 2KByte matches build
	
	--erase 30KByte of application code
--	erase_main()

	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	print("\n");

	--advance past the first 2KB of build
	dict.fwupdate("ERASE_1KB_PAGE", 30)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	print("\n");
	
	dict.fwupdate("SET_FLASH_ADDR", 0x7912, 0x01)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )

--	dict.fwupdate("UNLOCK_FLASH")

	dict.fwupdate("WR_HWORD", 0xCC33, 0x00)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_DATA") 
	print("flash data:", string.format("%X", rv) )
	print("\n");
	print("\n");

	dict.fwupdate("WR_HWORD", 0x1111, 0x01)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_DATA") 
	print("flash data:", string.format("%X", rv) )
	print("\n");

	dict.fwupdate("WR_HWORD", 0x2222, 0x01)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_DATA") 
	print("flash data:", string.format("%X", rv) )
	print("\n");

	dict.fwupdate("WR_HWORD", 0x4444, 0x02)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_DATA") 
	print("flash data:", string.format("%X", rv) )
	print("\n");

	dict.fwupdate("WR_HWORD", 0x7777, 0x03)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_DATA") 
	print("flash data:", string.format("%X", rv) )
	print("\n");

	dict.fwupdate("WR_HWORD", 0xAAAA, 0x10)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_DATA") 
	print("flash data:", string.format("%X", rv) )
	print("\n");

	dict.fwupdate("WR_HWORD", 0xBBBB, 0x20)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	rv = dict.fwupdate("GET_FLASH_DATA") 
	print("flash data:", string.format("%X", rv) )
	print("\n");

--	dict.fwupdate("LOCK_FLASH")

	rv = dict.fwupdate("READ_FLASH", 0x0000, 0x00) 
	print("read data:", string.format("%X", rv) )

	rv = dict.fwupdate("READ_FLASH", 0x053e, 0x00) 
	print("read data:", string.format("%X", rv) )

	rv = dict.fwupdate("READ_FLASH", 0x791a, 0x00) 
	print("read data:", string.format("%X", rv) )

	dict.fwupdate("RESET_DEVICE")

	--write build to flash

	print("updated")
end

-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
fwupdate.update_firmware = update_firmware

-- return the module's table
return fwupdate
