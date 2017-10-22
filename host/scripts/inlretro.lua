-- main script that runs application logic and flow



-- initial function called from C main
function main ()

	local dict = require "scripts.app.dict"
	local cart = require "scripts.app.cart"
	local nes = require "scripts.app.nes"
	local snes = require "scripts.app.snes"
	local dump = require "scripts.app.dump"
	local erase = require "scripts.app.erase"
	local flash = require "scripts.app.flash"
	local swim = require "scripts.app.swim"
--	local crc32 = require "scripts.app.crc32"

	local rv
--	rv = dict.pinport( "CTL_ENABLE" )
--	rv = dict.pinport( "CTL_IP_PU", "LED" )
--	rv = dict.pinport( "CTL_RD", "LED" )
--	rv = dict.pinport( "CTL_OP", "LED" )
--	rv = dict.pinport( "CTL_SET_HI", "LED" )
--	rv = dict.pinport( "CTL_RD", "LED" )
--	rv = dict.pinport( "DATA_ENABLE" )
--	rv = dict.pinport( "DATA_RD" )
--	rv = dict.pinport( "DATA_OP" )
--	rv = dict.pinport( "DATA_SET", 0xAA )
--	rv = dict.pinport( "DATA_RD" )
--	rv = dict.io("IO_RESET")
	if rv then
		print(string.format("%X", rv))
	end

--	print(dict.io("EXP0_PULLUP_TEST"))	
--


--	debug = true
--	rv = cart.detect(debug)

--	local force_cart = true
--	cart_console = "SNES"

	if (force_cart or cart.detect_console(true)) then
		if cart_console == "NES" or cart_console == "Famicom" then
			dict.io("IO_RESET")	
			dict.io("NES_INIT")	
			
			--NES detect mirroring to gain mapper info
			nes.detect_mapper_mirroring(true)
			--NES detect memories to gain more mapper/board info	
				--check for CHR-RAM
			nes.ppu_ram_sense(0x1000, true)
				--check CHR-ROM flash ID
			nes.read_flashID_chrrom_8K(true)
				--check for EXP0 pullup..?
			print("EXP0 pull-up test:", dict.io("EXP0_PULLUP_TEST"))	
				--try to read flash ID
			--setup to fail if 16KB banks
			print("32KB banks:")
			dict.nes("NES_CPU_WR", 0x8000, 0xFF)
			nes.read_flashID_prgrom_exp0(true)
			--setup to pass if 16KB banks
			print("16KB banks:")
			dict.nes("NES_CPU_WR", 0x8000, 0x00)
			nes.read_flashID_prgrom_exp0(true)
				--try mapper 30 flash ID

			--determined all that could about mapper board
			--set rom types and sizes
			--perform desired operation


			--FLASHING:
			--erase cart
			erase.erase_nes( true )
			--open file
			local file 
			file = assert(io.open("inltest.bin", "rb"))
			--determine if auto-doubling, deinterleaving, etc, 
			--needs done to make board compatible with rom
			--flash cart
			flash.flash_nes( file, true )
			--close file
			assert(file:close())

			--DUMPING:
			--create new file
			local file 
			file = assert(io.open("dump.bin", "wb"))
			--dump cart into file
			dump.dump_nes( file, true )

			--close file
			assert(file:close())


			dict.io("IO_RESET")	

		elseif cart_console == "SNES" then

			swim.start()

			dict.io("IO_RESET")	
			dict.io("SNES_INIT")	


			--SNES detect HiROM or LoROM 
			--nes.detect_mapper_mirroring(true)
			local snes_mapping = "LOROM"
			--SNES detect if there's save ram and size

			--SNES detect if able to read flash ID's
			if not snes.read_flashID(true) then
				print("ERROR unable to read flash ID")
				return
			end

			--quick lame check to see if chip erased
			if snes.read_reset_vector(0, true) ~= 0xFFFF then
				erase.erase_snes( false )
			end
			if snes.read_reset_vector(1, true) ~= 0xFFFF then
				erase.erase_snes( false )
			end
			if snes.read_reset_vector(20, true) ~= 0xFFFF then
				erase.erase_snes( false )
			end
			if snes.read_reset_vector(63, true) ~= 0xFFFF then
				erase.erase_snes( false )
			end


			--FLASHING:
			--erase cart
--			erase.erase_snes( false )
			--open file
			local file 
	--	      	file = assert(io.open("flash.bin", "rb"))
		      	file = assert(io.open("SF2_PTdump_capcomFINAL.bin", "rb"))

			--calculate checksum
			--local data = file:read("*all")
			--print(crc32.hash(data))

			--determine if auto-doubling, deinterleaving, etc, 
			--needs done to make board compatible with rom
			--flash cart
			flash.flash_snes( file, true )
			--close file
			assert(file:close())

			--DUMPING:
			--create new file
			local file 
			file = assert(io.open("snesdump.bin", "wb"))
			--dump cart into file
			dump.dump_snes( file, snes_mapping, true )

			--close file
			assert(file:close())


		--trick to do this at end while debugging so don't have to wait for it before starting
			erase.erase_snes( false )

			dict.io("IO_RESET")	

		elseif cart_console == "SegaGen" then

		elseif cart_console == "N64" then

		elseif cart_console == "DMG" then

		elseif cart_console == "GBA" then

		elseif cart_console == "SMS" then

		end

	end

end


main ()

