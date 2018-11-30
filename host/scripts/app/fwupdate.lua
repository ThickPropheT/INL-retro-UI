
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
--	while (curpage<128) do	--RB has 128KB but last 96KB isn't used (yet)
		if(curpage%4 ==0) then
			print("erasing page:", curpage)
		end
		dict.fwupdate("ERASE_1KB_PAGE", curpage)

		--rv = dict.fwupdate("GET_FLASH_ADDR") 
		--print("flash addr:", string.format("%X", rv) )

		curpage = curpage+1
	end

end

local function update_firmware(newbuild)

	print("updating")

	--open new file first, don't bother continuing if can't find it.
	file = assert(io.open(newbuild, "rb"))

	--TODO verify the first 2KByte match, don't continue if not..

	--enter fwupdate mode
	dict.bootload("PREP_FWUPDATE")	

	--now the device will only respond to FWUPDATE dictionary commands
	
	--erase 30KByte of application code
	erase_main()

	--Set FLASH->AR to beginging of application section
	--this can be done be re-erasing it..
	--maybe we could have skipped page 2 in erase_main
	--or have erase_main count down..
	dict.fwupdate("ERASE_1KB_PAGE", 2)
	rv = dict.fwupdate("GET_FLASH_ADDR") 
	print("flash addr:", string.format("%X", rv) )
	print("\n");
	
	--advance the file past first 2KByte
	local buffsize = 1
	local byte, data
	local byte_num = 0
	for byte in file:lines(buffsize) do
		data = string.unpack("B", byte, 1)
		--print(string.format("%X", data))
		byte_num = byte_num + 1
		if byte_num == 2048 then break end
	end

	local offset = 0
	local readdata
	local data_l
	buffsize = 1	 --2 bytes at a time
	print("Writting half word at at time, but it's not much data..")
	while byte_num < (32*1024) do

		--read next byte from the file and convert to binary
		--gotta be a better way to read a half word (16bits) at a time but don't care right now...
		byte_str = file:read(buffsize)
		if byte_str then
			data_l = string.unpack("B", byte_str, 1)
		else
			--should only have to make this check for lower byte
			--binary file should be even
			print("end of file")
			break
		end
		byte_str = file:read(buffsize)
		data = string.unpack("B", byte_str, 1)
		data = (data<<8)+data_l
	--	print("writting:", string.format("%X", data), "addr:", string.format("%X", byte_num))
	
		if( (byte_num % (4*1024)) == 0 ) then
			print("flashing KB", byte_num/1024)
		end

		--write the data
		dict.fwupdate("WR_HWORD", data, offset)

--		if (verify) then
--			readdata = dict.fwupdate("READ_FLASH", byte_num, 0x00) 
--			print("read data:", string.format("%X", rv) )
--			if readdata ~= data then
--				print("ERROR flashing byte number", byte_num, " to flash ", data, readdata)
--			end
--		end

		offset = 1 --this is zero for first byte, but one for all others..
		byte_num = byte_num + 2
	end
	--]]

--	dict.fwupdate("WR_HWORD", 0xCC33, 0x00)
--	rv = dict.fwupdate("GET_FLASH_ADDR") 
--	print("flash addr:", string.format("%X", rv) )
--	rv = dict.fwupdate("GET_FLASH_DATA") 
--	print("flash data:", string.format("%X", rv) )
--	print("\n");
--	print("\n");
--
--	dict.fwupdate("WR_HWORD", 0x1111, 0x01)
--	rv = dict.fwupdate("GET_FLASH_ADDR") 
--	print("flash addr:", string.format("%X", rv) )
--	rv = dict.fwupdate("GET_FLASH_DATA") 
--	print("flash data:", string.format("%X", rv) )
--	print("\n");
--
--	dict.fwupdate("WR_HWORD", 0x2222, 0x01)
--	rv = dict.fwupdate("GET_FLASH_ADDR") 
--	print("flash addr:", string.format("%X", rv) )
--	rv = dict.fwupdate("GET_FLASH_DATA") 
--	print("flash data:", string.format("%X", rv) )
--	print("\n");
--
--	dict.fwupdate("WR_HWORD", 0x4444, 0x02)
--	rv = dict.fwupdate("GET_FLASH_ADDR") 
--	print("flash addr:", string.format("%X", rv) )
--	rv = dict.fwupdate("GET_FLASH_DATA") 
--	print("flash data:", string.format("%X", rv) )
--	print("\n");
--
--	dict.fwupdate("WR_HWORD", 0x7777, 0x03)
--	rv = dict.fwupdate("GET_FLASH_ADDR") 
--	print("flash addr:", string.format("%X", rv) )
--	rv = dict.fwupdate("GET_FLASH_DATA") 
--	print("flash data:", string.format("%X", rv) )
--	print("\n");
--
--	dict.fwupdate("WR_HWORD", 0xAAAA, 0x10)
--	rv = dict.fwupdate("GET_FLASH_ADDR") 
--	print("flash addr:", string.format("%X", rv) )
--	rv = dict.fwupdate("GET_FLASH_DATA") 
--	print("flash data:", string.format("%X", rv) )
--	print("\n");
--
--	dict.fwupdate("WR_HWORD", 0xBBBB, 0x20)
--	rv = dict.fwupdate("GET_FLASH_ADDR") 
--	print("flash addr:", string.format("%X", rv) )
--	rv = dict.fwupdate("GET_FLASH_DATA") 
--	print("flash data:", string.format("%X", rv) )
--	print("\n");
--
----	dict.fwupdate("LOCK_FLASH")
--
--	rv = dict.fwupdate("READ_FLASH", 0x0000, 0x00) 
--	print("read data:", string.format("%X", rv) )
--
--	rv = dict.fwupdate("READ_FLASH", 0x053e, 0x00) 
--	print("read data:", string.format("%X", rv) )
--
--	rv = dict.fwupdate("READ_FLASH", 0x791a, 0x00) 
--	print("read data:", string.format("%X", rv) )

	--close file
	assert(file:close())
	print("\n\n DONE updating flash. \n\n Reseting device \n\n IGNORE the error that comes next.. \n\n")

	dict.fwupdate("RESET_DEVICE")

	--write build to flash

	print("updated")	--this doesn't print because reset errored us out..
end

-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
fwupdate.update_firmware = update_firmware

-- return the module's table
return fwupdate
