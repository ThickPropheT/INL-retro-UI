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
	dict.io("IO_RESET")	

	dict.io("SNES_INIT")	
	dict.io("SWIM_INIT", "SWIM_ON_EXP0")	
	dict.swim("SWIM_ACTIVATE")	

	--holds SWIM pin low for 16usec+ to reset SWIM comms incase of error
      	dict.swim("SWIM_RESET")	


	--write 0A0h to SWIM_CSR
	--bit 5: allows entire memory range to be read & swim reset to be accessed
	--bit 7: masks internal reset sources (like WDT..?)
	print("wotf SWIM_CSR:", dict.swim("WOTF", 0x7F80, 0xA0))	

	--read SWIM_CSR
	print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
	print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
	print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
	print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
	print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
      	dict.swim("SWIM_RESET")	

	print("wotf SRST:", dict.swim("SWIM_SRST"))	
	print("wotf SWIM_CSR:", dict.swim("WOTF", 0x7F80, 0xA0))	

	--now the SRST command is available, whole memory range available, and internal resets disabled
	--by default there is now a breakpoint set at reset vector
	
	--reset the STM8 core
	print("wotf SRST:", dict.swim("SWIM_SRST"))	

	--the STM8 core is now stalled @ reset vector
	--can read/write to any address on STM8 core
	--if CIC ROP bit is set, we can only r/w to periph & SRAM

	--bit 2: SWIM is reset (exits active mode) when chip reset
	--this forces successful SWIM entry on each execution of script
	print("wotf SWIM_CSR:", dict.swim("WOTF", 0x7F80, 0xA4))	

	--test by blinking LED via periph register access
	--v2 board has LED on hi_lo_sel PA2
	print("wotf LED PA_CR1:", dict.swim("WOTF", 0x5003, 0xFF))	--default is input w/o pullup, now pullups enabled
	--LED should be dimly lit
	--set pin to pushpull
	print("wotf LED PA_DDR:", dict.swim("WOTF", 0x5002, 0x04))	--PA2 is output CR1 set above makes pushpull
	--LED is push/pull, ODR default to 0, so LED OFF
	print("wotf LED PA_ODR:", dict.swim("WOTF", 0x5000, 0x04))	--PA2 output set LED ON!
	print("wotf LED PA_ODR:", dict.swim("WOTF", 0x5000, 0x00))	--PA2 output set LED OFF!


	--holds SWIM pin low for 16usec+ to reset SWIM comms incase of error
	dict.swim("SWIM_RESET")	

	--reset the chip, if bit2 set in CSR the SWIM exits active mode with this reset
	print("wotf SRST:", dict.swim("SWIM_SRST"))	
	--SWIM is now inactive chip is executing it's program code

	--indicate to logic analyzer that test sequence above is complete
	dict.pinport("CTL_SET_LO", "EXP0")
	dict.io("IO_RESET")	

--	debug = true
--	rv = cart.detect(debug)

	local force_cart = true

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
			file = assert(io.open("flash.bin", "rb"))
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
			dict.io("IO_RESET")	
			dict.io("SNES_INIT")	


			--SNES detect HiROM or LoROM 
			--nes.detect_mapper_mirroring(true)
			local snes_mapping = "LOROM"
			--SNES detect if there's save ram and size

			--SNES detect if able to read flash ID's
			snes.read_flashID(true)



			--FLASHING:
			--erase cart
	--		erase.erase_snes( false )
			--open file
			local file 
		      	file = assert(io.open("flash.bin", "rb"))
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
--			erase.erase_snes( false )

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

