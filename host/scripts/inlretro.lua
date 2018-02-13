-- main script that runs application logic and flow



-- initial function called from C main
function main ()


	print("\n")

	local dict = require "scripts.app.dict"
	local cart = require "scripts.app.cart"
	local nes = require "scripts.app.nes"
	local snes = require "scripts.app.snes"
	local dump = require "scripts.app.dump"
	local erase = require "scripts.app.erase"
	local flash = require "scripts.app.flash"
	local swim = require "scripts.app.swim"
	local jtag = require "scripts.app.jtag"
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

	local force_cart = true
	cart_console = "NES"

	if (force_cart or cart.detect_console(true)) then
		if cart_console == "NES" or cart_console == "Famicom" then
			dict.io("IO_RESET")	
			dict.io("NES_INIT")	
			
		--[[
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
		--]]


			jtag.run_jtag()


			--Check for SWIM on A0
		--[[
			dict.io("IO_RESET")	
			print("start swim")

			dict.io("SWIM_INIT", "SWIM_ON_A0")	
			if swim.start(true) then
				--SWIM is now established and running at HIGH SPEED
				snes_swimcart = false	--don't want to use SWIM pin to control flash /OE, use SNES RESET (EXP0) instead

			--	swim.swim_test()

				--swim.write_optn_bytes( true, true ) -- enable ROP, debug

				--check if ROP set, allow clearing ROP and erasing CIC
				--blindly erase STM8 CIC for now by disabling ROP
				swim.disable_ROP_erase(true)
				
				--open CIC file
				--local cic_file = assert(io.open("stm8_8KB_zero.bin", "rb"))
				--local cic_file = assert(io.open("stm8_8KB_0xff.bin", "rb"))
				--local cic_file = assert(io.open("stm8_8KB_testpattern.bin", "rb"))
				local cic_file = assert(io.open("NESCIC.bin", "rb"))

				--write CIC file
				swim.write_flash( cic_file )

				--close CIC file
				assert(cic_file:close())

				--set ROP & AFR0
				swim.write_optn_bytes( false, true ) -- ROP not set, debug set

				-- reset STM8 CIC and end SWIM comms to it can execute what we just flashed
				swim.stop_and_reset()
			else
				print("ERROR problem with STM8 CIC")
			end

			print("done flashing STM8 on A0")

			dict.io("IO_RESET")	
		--]]


			--determined all that could about mapper board
			--set rom types and sizes
			--perform desired operation


		--[[
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
		--]]


	--		dict.io("IO_RESET")	

		elseif cart_console == "SNES" then

			snes_swimcart = nil
			if swim.start(true) then
				--SWIM is now established and running at HIGH SPEED
				snes_swimcart = false	--don't want to use SWIM pin to control flash /OE, use SNES RESET (EXP0) instead

				--check if ROP set, allow clearing ROP and erasing CIC
				--blindly erase STM8 CIC for now by disabling ROP
				swim.disable_ROP_erase(true)
				
				--open CIC file
				local cic_file = assert(io.open("SNESCIC_flashmode.bin", "rb"))

				--write CIC file
				swim.write_flash( cic_file )

				--close CIC file
				assert(cic_file:close())

				-- reset STM8 CIC and end SWIM comms to it can execute what we just flashed
				swim.stop_and_reset()
			else
				print("ERROR problem with STM8 CIC")
			end

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
			if snes.read_reset_vector( 1, true) ~= 0xFFFF then
				erase.erase_snes( false )
			end
			if snes.read_reset_vector( 20, true) ~= 0xFFFF then
				erase.erase_snes( false )
			end
			if snes.read_reset_vector( 63, true) ~= 0xFFFF then
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

			--flash final CIC code
			if swim.start(true) then
				--SWIM is now established and running at HIGH SPEED
				--swim.printCSR()
				snes_swimcart = false
				--print("main swimcart", snes_swimcart)

				--check if ROP set, allow clearing ROP and erasing CIC
				--blindly erase STM8 CIC for now by disabling ROP
		--		swim.disable_ROP_erase(true)
				
				--open CIC file
				local cic_file = assert(io.open("SNESCIC.bin", "rb"))

				--write CIC file
				swim.write_flash( cic_file )

				--close CIC file
				assert(cic_file:close())

				--write option bytes
						    -- enable ROP, debug
				swim.write_optn_bytes( true, true )

				-- reset STM8 CIC and end SWIM comms to it can execute what we just flashed
				swim.stop_and_reset()
			else
				print("ERROR problem with STM8 CIC")
			end

--			--DUMPING:
--			--create new file
--			local file 
--			file = assert(io.open("snesdump.bin", "wb"))
--			--dump cart into file
--		--	swim.start()
--			dump.dump_snes( file, snes_mapping, true )
--
--			--close file
--			assert(file:close())


		--trick to do this at end while debugging so don't have to wait for it before starting
	--		erase.erase_snes( false )

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

