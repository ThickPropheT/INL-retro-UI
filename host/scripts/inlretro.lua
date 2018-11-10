-- main script that runs application logic and flow

-- =====================================================
-- USER NOTES
-- =====================================================
-- 1- set 'curcart' to point to desired mapper script (around line 60 currently)
-- 2- set 'cart_console' to the currently inserted cartridge (around line 80 currently)
-- 	this will control flow of the script later on which is the 
-- 	location of what you'll need to modify in the next step.
-- 3- call curcart.process function to actually run something:
-- 	NES for example, this is done around line 270ish currently..
-- 	Here are a few NES NROM examples:
--
--	--NROM test & dump to dump.bin file
--	curcart.process( true, true, false, false, false, "ignore/dump.bin", nil, nil)
--
--	--NROM test, erase, & flash flash.bin file
--	curcart.process( true, false, true, true, false, nil, "ignore/flash.bin", nil)
--
--	--NROM test, dump (to dump.bin), then erase.  Next flash flash.bin, lastly dump again to verify.bin
--	curcart.process( true, true, true, true, true, "ignore/dump.bin", "ignore/flash.bin, "ignore/verify.bin)
--
--	Here is the nrom.process function definition:
--	local function process( test, read, erase, program, verify, dumpfile, flashfile, verifyfile)
--	arg 1 - test: this will run some tests on the cart to help determine things like mirroring & flash type
--	arg 2 - read: this will dump the rom memories on the cartridge to 'dumpfile', (done before subequent steps)
--	*The remaining args are only for flash boards purchased from our site:
--	arg 3 - erase: this will erase flash roms on the cartridge
--	arg 4 - program: this will write 'flashfile' to the cartridge
--	arg 5 - verify: this will dump the memories to 'verifyfile', just like read could/did, but done last.
--	arg 6,7,8 files: The relative path of where the files can be found/created from steps above.
--			 You don't have to set unused file names to nil, that was just done for the examples.
--
-- =====================================================


-- initial function called from C main
function main ()


	print("\n")

	local dict = require "scripts.app.dict"
	local cart = require "scripts.app.cart"
	local nes = require "scripts.app.nes"
	local snes = require "scripts.app.snes"
--	local gameboy = require "scripts.app.gameboy"
--	local gba = require "scripts.app.gba"
--	local sega = require "scripts.app.sega"
--	local n64 = require "scripts.app.n64"
	local dump = require "scripts.app.dump"
	local erase = require "scripts.app.erase"
	local flash = require "scripts.app.flash"
	local swim = require "scripts.app.swim"
	local jtag = require "scripts.app.jtag"
	local ciccom = require "scripts.app.ciccom"
--	local bootload = require "scripts.app.bootload"
--	local crc32 = require "scripts.app.crc32"


-- =====================================================
-- USERS: set curcart to point to the mapper script you would like to use here.
--	The -- comments out a line, so you can add/remove the -- to select/deselect mapper scripts
-- =====================================================
	--cart/mapper specific scripts
	--local curcart = require "scripts.nes.nrom"
	--local curcart = require "scripts.nes.cnrom"
	--local curcart = require "scripts.nes.mmc1"
	local curcart = require "scripts.nes.mmc3"
	--local curcart = require "scripts.nes.unrom"
	--local curcart = require "scripts.nes.mm2"
	--local curcart = require "scripts.nes.mapper30"
	--local curcart = require "scripts.nes.bnrom"
	--local curcart = require "scripts.nes.cdream"
	--local curcart = require "scripts.nes.cninja"
	--local curcart = require "scripts.nes.action53"
	--local curcart = require "scripts.nes.action53_tsop"
	--local curcart = require "scripts.nes.easyNSF"
	--local curcart = require "scripts.nes.dualport"
	--local curcart = require "scripts.snes.v3"  --and GAMEBOY for now
	--local curcart = require "scripts.snes.lorom_5volt"  --catskull design
	--local curcart = require "scripts.snes.v2proto"
	
-- =====================================================
-- USERS: set cart_console to the  to point to the mapper script you would like to use here.
-- =====================================================
	local cart_console = "NES" 	--includes Famicom
	--local cart_console = "SNES"
	--local cart_console = "SEGA"
	--local cart_console = "N64"
	--local cart_console = "DMG"
	--local cart_console = "GBA"
	--local cart_console = "SMS"

	local rv
--	rv = dict.pinport( "DATA_SET", 0xAA )
--	rv = dict.pinport( "DATA_RD" )
--	rv = dict.io("IO_RESET")

--[[	--TEST GB power
	rv = dict.pinport( "CTL_ENABLE", "CIN" )
	rv = dict.pinport( "CTL_OP", "CIN")
	rv = dict.pinport( "CTL_SET_LO", "CIN")
	print("CIN low 5v GB")
	jtag.sleep(5)
	rv = dict.pinport( "CTL_SET_HI", "CIN")
	print("CIN high 3v GBA")
	jtag.sleep(90)
	rv = dict.pinport( "CTL_SET_LO", "CIN")
	print("CIN low 5v GB")
	jtag.sleep(10)
	rv = dict.pinport( "CTL_SET_HI", "CIN")
	print("CIN high 3v GBA")
	jtag.sleep(10)
	--]]


	--if rv then
	--	print(string.format("%X", rv))
	--end

--	print(dict.io("EXP0_PULLUP_TEST"))	
--

	--BOOTLOADER TEST
	--print("jumping...")

	--jump to 0xDEADBEEF
	--dict.bootload("LOAD_ADDRH", 0xDEAD)	
	--dict.bootload("JUMP_ADDR",  0xBEEF)	--for ARM LSbit must be odd (Thumb mode)
--	dict.bootload("LOAD_ADDRH", 0x1FFF)	
--	dict.bootload("JUMP_ADDR",  0xCAC5)	
--	dict.bootload("LOAD_ADDRH", 0x0800)	
--	dict.bootload("JUMP_ADDR",  0x00C1)	

	--dict.bootload("JUMP_BL")	
	--print("jumped")

--	debug = true
--	rv = cart.detect(debug)

--DETECT WHICH CART IS INSERTED, 
--or take user input for manual override 
--VERIFY BASIC CART FUNCTIONALITY
--DON'T PUT THE CART IN ANY WEIRD STATE LIKE SWIM ACTIVATION OR ANYTHING
--	IF SOMETHING LIKE THIS IS DONE, IT MUST BE UNDONE PRIOR TO MOVING ON

--PROCESS USER ARGS ON WHAT IS TO BE DONE WITH CART

	local force_cart = true

	if (force_cart or cart.detect_console(true)) then
		if cart_console == "NES" or cart_console == "Famicom" then
			dict.io("IO_RESET")	
			dict.io("NES_INIT")	
			
		---[[
			--NES detect mirroring to gain mapper info
			nes.detect_mapper_mirroring(true)
		--[[
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
			print("\n")
		--]]


		--	jtag.run_jtag()


			--Check for SWIM on A0
		--[[
			dict.io("IO_RESET")	
			print("start swim")

			dict.io("SWIM_INIT", "SWIM_ON_A0")	
			----[[
			if swim.start(true) then
				--SWIM is now established and running at HIGH SPEED
				snes_swimcart = false	--don't want to use SWIM pin to control flash /OE, use SNES RESET (EXP0) instead

				swim.swim_test()

				--swim.write_optn_bytes( true, true ) -- enable ROP, debug

				--check if ROP set, allow clearing ROP and erasing CIC
				--blindly erase STM8 CIC for now by disabling ROP
				swim.disable_ROP_erase(true)
				
				--open CIC file
				local cic_file = assert(io.open("stm8_8KB_zero.bin", "rb"))
				--local cic_file = assert(io.open("stm8_8KB_0xff.bin", "rb"))
				--local cic_file = assert(io.open("stm8_8KB_testpattern.bin", "rb"))
				--local cic_file = assert(io.open("NESCIC.bin", "rb"))
				--local cic_file = assert(io.open("LIZv1.bin", "rb"))

				--write CIC file
				swim.write_flash( cic_file )

				--close CIC file
				assert(cic_file:close())

				--set ROP & AFR0
				swim.write_optn_bytes( true, true ) -- ROP not set, debug set

				-- reset STM8 CIC and end SWIM comms to it can execute what we just flashed
				swim.stop_and_reset()
			else
				print("ERROR problem with STM8 CIC")
			end

			print("done flashing STM8 on A0")

			dict.io("IO_RESET")	

			--]]

		--[[
		

			ciccom.start()
			ciccom.set_opcode("M")
			--now send operand "V" (0x56) or "H" (0x48)
			ciccom.write("H")

			

			dict.io("IO_RESET")	
			jtag.sleep(0.01) --10msec to be overly safe


		---[[
			--the CIC won't update the H/V stack flag unless it's reset
			--reset caused during swim init doesn't count either bc it halts at reset vector

			--test reading back CIC version
			dict.io("SWIM_INIT", "SWIM_ON_A0")	
			--dict.io("SWIM_INIT", "SWIM_ON_EXP0")	
			if swim.start(true) then

				swim.read_stack()

			else
				print("ERROR trying to read back CIC signature stack data")
			end
			swim.stop_and_reset()

			print("done reading STM8 stack on A0\n")

			dict.io("IO_RESET")	
		--]]


			--determined all that could about mapper board
			--set rom types and sizes
			--perform desired operation
			--CART and programmer should be in a RESET condition upon calling the specific script

			--DUALPORT
			--curcart.process( true, false, false, false, false, "ignore/dump.bin", "ignore/ddug2.bin", "ignore/verifyout.bin")
			--MMC1
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/BB_sgrom.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/Zelda2.bin", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/Zelda2_doubleprg.bin", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/alfonzoMMC1.bin", "ignore/verifyout.bin")

			--UxROM
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/AFB_128.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/nomolosFINAL.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/owlia_revb.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, false, false, false, "ignore/dump.bin", "ignore/rushnattack.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, false, false, false, "ignore/dump.bin", "ignore/TDfix.prg", "ignore/verifyout.bin")


			--MM2
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/mm2_i0.prg", "ignore/verifyout.bin")
			--curcart.process( true, true, false, false, false, "ignore/dump.bin", "ignore/mm2_i0.prg", "ignore/verifyout.bin")

			--UNROM512 mapper 30
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/BBC_nonJW.bin", "ignore/verifyout.bin")
			--curcart.process( true, false, false, false, false, "ignore/dump.bin", "ignore/MysticOrigins.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/NESmaker.nes", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/tb_map30.prg", "ignore/verifyout.bin")


			--COLOR NINJA
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/ninja.bin", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/alfonzo.bin", "ignore/verifyout.bin")
			
			--A53 PLCC
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/a53v1_SBR2.prg", "ignore/verifyout.bin")
			--A53 tssop
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/a53vol3.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/2a03puritans_RE.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/a53vol3_giftmsg_0xFF.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, false, false, false, "ignore/dump.bin", "ignore/writethe actual message.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/mappertests/test28-8Mbit.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/mappertests/M28_P1M_CR32K.prg", "ignore/verifyout.bin")

			--easy NSF tssop
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/2a03puritans_RE.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/rndm2_1MB.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/pico2015_RELEASE_1MB.prg", "ignore/verifyout.bin")




			--later scripts which don't require specific firmware functions
			--goal is to convert scripts above to be more like the ones below now that
			--have a better idea of what works best and minimizing firmware compilation and updates

			--NROM
			--curcart.process( true, true, true, true, true, "ignore/dump.bin", "ignore/ddug2.bin", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/NTB_RE.bin", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/MM_demo.bin", "ignore/verifyout.bin")
			--curcart.process( true, true, true, true, true, "ignore/dump.bin", "ignore/NnD.bin", "ignore/verifyout.bin", "V")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/DEMO.bin", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/NES_hb_present.bin", "ignore/verifyout.bin")

			--CNROM
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/galf.bin", "ignore/verifyout.bin")

			--MMC3
			--curcart.process( true, true, true, false, true, "ignore/dump.bin", "ignore/kirby.nes", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/kirby.bin", "ignore/verifyout.bin", false, false, "ignore/ramdump.bin", "ignore/ramwrite.bin")
			curcart.process( true, false, false, false, false, "ignore/dump.bin", "ignore/kirby.bin", "ignore/verifyout.bin", true, true, "ignore/ramdump.bin", "ignore/kirby3xSave.bin")

			--COLOR DREAMS
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/multicart_mojontalesFINAL.prg", "ignore/verifyout.bin")

			--BNROM
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/lizard_PG.prg", "ignore/verifyout.bin")
			--curcart.process( true, true, true, true, true, "ignore/dump.bin", "ignore/lizard_v2.prg", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, false, "ignore/dump.bin", "ignore/hh85.prg", "ignore/verifyout.bin")

		--[[
			--FLASHING:
			--erase cart
			erase.erase_nes( true )
			--open file
			local file 
			--file = assert(io.open("inltest.bin", "rb"))
			--file = assert(io.open("ignore/ddug2.bin", "rb"))
			file = assert(io.open("ignore/lizard_v1.bin", "rb"))
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

			--new SNES code 
			
			--SNES
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/MMXdump.bin", "ignore/verifyout.bin")
			curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/smw.sfc", "ignore/verifyout.bin")
			--curcart.process( true, true, false, false, false, "ignore/dump.bin", "ignore/hsbm_4Mbit_Lo.sfc", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/hsbm_4Mbit_Lo.sfc", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/hsbm_4Mbit_Hi.sfc", "ignore/verifyout.bin")
			--curcart.process( true, false, true, true, true, "ignore/dump.bin", "ignore/hsbm_32Mbit_Hi.sfc", "ignore/verifyout.bin")


--			--old SNES code
--		
--			snes_swimcart = nil
--			dict.io("IO_RESET")	
--			print("start swim")
--
--			dict.io("SWIM_INIT", "SWIM_ON_A0")	
--			--[[
--			if swim.start(true) then
--				--SWIM is now established and running at HIGH SPEED
--				snes_swimcart = false	--don't want to use SWIM pin to control flash /OE, use SNES RESET (EXP0) instead
--			print("swim prgm mode")
--
--				--check if ROP set, allow clearing ROP and erasing CIC
--				--blindly erase STM8 CIC for now by disabling ROP
--				swim.disable_ROP_erase(true)
--				
--				--open CIC file
--				local cic_file = assert(io.open("SNESCIC_flashmode.bin", "rb"))
--
--				--write CIC file
--				swim.write_flash( cic_file )
--
--				--close CIC file
--				assert(cic_file:close())
--
--				-- reset STM8 CIC and end SWIM comms to it can execute what we just flashed
--				swim.stop_and_reset()
--			else
--				print("ERROR problem with STM8 CIC")
--			end
--			--]]
--
--			dict.io("IO_RESET")	
--			dict.io("SNES_INIT")	
--
--
--			--SNES detect HiROM or LoROM 
--			--nes.detect_mapper_mirroring(true)
--			local snes_mapping = "LOROM"
--			--SNES detect if there's save ram and size
--
--		---[[
--			--SNES detect if able to read flash ID's
--			if not snes.read_flashID(true) then
--				print("ERROR unable to read flash ID")
--				return
--			end
--
--			erase.erase_snes( false )
--			--quick lame check to see if chip erased
--			---[[
--			if snes.read_reset_vector(0, true) ~= 0xFFFF then
--				erase.erase_snes( false )
--			end
--			if snes.read_reset_vector( 1, true) ~= 0xFFFF then
--				erase.erase_snes( false )
--			end
--			if snes.read_reset_vector( 20, true) ~= 0xFFFF then
--				erase.erase_snes( false )
--			end
--			if snes.read_reset_vector( 63, true) ~= 0xFFFF then
--				erase.erase_snes( false )
--			end
--			--]]
--
--			--FLASHING:
--			--erase cart
----			erase.erase_snes( false )
--			--open file
--			local file 
--	---[[      	file = assert(io.open("flash.bin", "rb"))
--		      	--file = assert(io.open("SF2_PTdump_capcomFINAL.bin", "rb"))
--		      	file = assert(io.open("ignore/MMXdump.bin", "rb"))
--
--			--calculate checksum
--			--local data = file:read("*all")
--			--print(crc32.hash(data))
--
--			--determine if auto-doubling, deinterleaving, etc, 
--			--needs done to make board compatible with rom
--			--flash cart
--			flash.flash_snes( file, true )
--			--close file
--			assert(file:close())
--			--]]
--
--			dict.io("IO_RESET")	
--			dict.io("SNES_INIT")	
--			print("start swim")
--
--		--[[
--			dict.io("SWIM_INIT", "SWIM_ON_A0")	
--			--flash final CIC code
--			if swim.start(true) then
--				--SWIM is now established and running at HIGH SPEED
--				--swim.printCSR()
--				--snes_swimcart = false
--				--print("main swimcart", snes_swimcart)
--
--				print("swimming")
--				swim.swim_test()
--
--				--check if ROP set, allow clearing ROP and erasing CIC
--				--blindly erase STM8 CIC for now by disabling ROP
--		--		swim.disable_ROP_erase(true)
--				
--				--open CIC file
--				local cic_file = assert(io.open("SNESCIC.bin", "rb"))
--
--				--write CIC file
----				swim.write_flash( cic_file )
--
--				--close CIC file
--				assert(cic_file:close())
--
--				--write option bytes
--						    -- enable ROP, debug
----				swim.write_optn_bytes( false, true )
--
--				--read stack
--				swim.read_stack(true)
--
--				-- reset STM8 CIC and end SWIM comms to it can execute what we just flashed
--				swim.stop_and_reset()
--
--			else
--				print("ERROR problem with STM8 CIC")
--			end
--		--]]
--
--
--			--DUMPING:
--			--create new file
--			print("dumping SNES")
--			snes.read_reset_vector(0, true)
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

		elseif cart_console == "SEGA" then

		elseif cart_console == "N64" then

		elseif cart_console == "DMG" then

			print("testing gameboy")

			--SNES should be similar
			curcart.process( false, true, false, false, false, "ignore/dump.bin", "ignore/gameboy.bin", "ignore/verifyout.bin")
			---[[	--TEST GB power
				rv = dict.pinport( "CTL_ENABLE", "GBP" )
				rv = dict.pinport( "CTL_OP", "GBP")
				rv = dict.pinport( "CTL_SET_HI", "GBP")
				print("GBP high 3v GBA")
				jtag.sleep(1)
				rv = dict.pinport( "CTL_SET_LO", "GBP")
				print("GBP low 5v GB")
				jtag.sleep(1)
				rv = dict.pinport( "CTL_SET_HI", "GBP")
				print("GBP high 3v GBA")
				jtag.sleep(1)
				rv = dict.pinport( "CTL_SET_LO", "GBP")
				print("GBP low 5v GB")
				--jtag.sleep(2)
				--]]
			

		elseif cart_console == "GBA" then

		elseif cart_console == "SMS" then

		end

	end


end


main ()

