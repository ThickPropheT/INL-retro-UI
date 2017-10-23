
-- create the module's table
local swim = {}

-- import required modules
local dict = require "scripts.app.dict"
--local buffers = require "scripts.app.buffers"

-- file constants
-- firmware assembly return error code definitions
--.equ    NO_RESP, 0xFF device didn't appear to respond                              
--.equ    ACK,    0x01  transfer successful
--.equ    NAK,    0x00  device couldn't complete operation                                      
--.equ    HERR,   0x0E  header error
--.equ    PERR,   0x09  pairity error
local ECODE = {}
ECODE.NORESP = 0xFF
ECODE.ACK = 0x01
ECODE.NAK = 0x00
ECODE.PERR = 0x0E
ECODE.HERR = 0x09
--local NRESP = 0xFF
--local ACK = 0x01
--local NAK = 0x00
--local PERR = 0x0E
--local HERR = 0x09

local cur_CSR = 0x00
local SWIM_CSR = 0x7F80

-- local functions
local function get_key_for_value( t, value )
	for k,v in pairs(t) do
		if v==value then 
			return k
		end
	end
	return nil
end

local function system_reset()

	--TODO if cur_CSR has bit 2 set, SWIM must be reactivated 
	
	if dict.swim("SWIM_SRST") ~= ECODE.ACK then
		print("ERROR unable to reset STM8 core")
	else
--		print("reset stm8 core")
	end
end

local function reset_swim()
	--print("resetting SWIM")
      	dict.swim("SWIM_RESET")	

--	wotf(SWIM_CSR, cur_CSR)
	--must rewrite current value of SWIM_CSR register as HIGHSPEED is cleared during SWIM RESET
      	dict.swim("WOTF", SWIM_CSR, cur_CSR)	
end

local function rotf(addr, hspeed, debug)

	local result = ECODE.NAK
	local data
	local tries = 5
	local resets = 3

	local opcode = "ROTF"
	if hspeed then
		opcode = "ROTF_HS"
	end

	while result ~= "ACK" and tries > 0 do
		result, data = dict.swim(opcode, addr)
		--convert the value to the key string
		result = get_key_for_value( ECODE, result)
		if debug then print("rotf", string.format(" %X: %X, result ", addr, data), result) end
		if result == "NORESP" then
			reset_swim()
		end
		tries = tries - 1
		if tries == 0 then 
			print("ERROR max tries exceeded") 
			reset_swim()
			resets = resets - 1
			if resets > 0 then
				tries = 5
			end
		end
	end
	
	--return the result of the final transfer
	return result, data
end

local function wotf(addr, data, hspeed, debug)

	local result = ECODE.NAK
	local tries = 8
	local resets = 3

	local opcode = "WOTF"
	if hspeed then
		opcode = "WOTF_HS"
	end

	while result ~= "ACK" and tries > 0 do
		result = dict.swim(opcode, addr, data)
		result = get_key_for_value( ECODE, result)
		if debug then print("wotf", string.format(" %X: %X, result ", addr, data), result) end
		if result == "NORESP" then
			reset_swim()
		end
		if tries == 0 then 
			print("ERROR max tries exceeded") 
			reset_swim()
			resets = resets - 1
			if resets > 0 then
				tries = 5
			end
		end
		tries = tries - 1
	end

	--return the result of the final transfer
	return result
end

local function unlock_eeprom(hspeed)
	--Write 0xAE then 56h in
	--FLASH_DUKR (0x00 5064)(1)(2)
	wotf(0x5064, 0xAE, hspeed) 
	wotf(0x5064, 0x56, hspeed) 
end

local function unlock_flash(hspeed)
	--write 0x56 then 0xae in
	--flash_pukr (0x00 5062)(3)
	wotf(0x5062, 0x56, hspeed) 
	wotf(0x5062, 0xAE, hspeed) 
end

local function lock_flash_eeprom(hspeed)
	--lock eeprom:
	--Reset bit 3 (DUL)
	--in FLASH_IAPSR (0x00 505F)
	--lock flash:
        --Reset bit 1 (PUL)
	--in FLASH_IAPSR (0x00 505F)
	--just lock em both
	wotf(0x505F, 0x00, hspeed) 
end

local function swim_test()
	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8028)))
	--print("wotf :", dict.swim("WOTF_HS", 0x8028, 0x49))
	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8028)))

--read then write to SRAM
--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x0000)))

      	--print("wotf :", dict.swim("WOTF_HS", 0x0000, 0x00))
	--high speed now, enable flag with true
--	wotf(0x0000, 0x00, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	rotf(0x0000, true) 
--	wotf(0x0000, 0xEE, true) 
--	rotf(0x0000, true) 
--	wotf(0x0000, 0xAA, true) 
--	rotf(0x0000, true) 
--	wotf(0x0000, 0x55, true) 
--	rotf(0x0000, true) 

--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x0000)))

--read then write to eeprom
--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x4000)))
--
--      --need to unlock the eeprom first!
	unlock_eeprom(true)
--	--Write 0xAE then 56h in
--	--FLASH_DUKR (0x00 5064)(1)(2)
--      	print("wotf :", dict.swim("WOTF_HS", 0x5064, 0xAE))
--      	print("wotf :", dict.swim("WOTF_HS", 0x5064, 0x56))
--	--write data
	rotf(0x4000, true)
	wotf(0x4000, 0xDE, true)
	wotf(0x4001, 0xAD, true)
	wotf(0x4002, 0xBE, true)
	wotf(0x4003, 0xEF, true)
--      	print("wotf :", dict.swim("WOTF_HS", 0x4000, 0x00))
--
--	--lock eeprom
--	--Reset bit 3 (DUL)
	lock_flash_eeprom(true)
--	--in FLASH_IAPSR (0x00 505F)
--      	print("wotf :", dict.swim("WOTF_HS", 0x505F, 0x00))
--
--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x4000)))
	rotf(0x4000, true)
	rotf(0x4001, true)
	rotf(0x4002, true)
	rotf(0x4003, true)

--read then write to flash
--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8028)))

	--need to unlock the flash first!
	unlock_flash(true)
	--write data
	print("WRITE DATA")
	local byte_addr = 0x8028
	local data = 0xFF
	while byte_addr < 0x8030 do
		wotf(byte_addr, data, true, true) 
	
		byte_addr = byte_addr + 1
--		data = data + 0x11

	end
	--lock flash/eeprom
	lock_flash_eeprom(true)
	--read it back
	print("READ BACK DATA")
	local byte_addr = 0x8028
	while byte_addr < 0x8030 do
		rotf(byte_addr, true, true)
	
		byte_addr = byte_addr + 1
	end

	--test by blinking LED via periph register access
	--v2 board has LED on hi_lo_sel PA2
--	print("wotf LED PA_CR1:", dict.swim("WOTF", 0x5003, 0xFF))	--default is input w/o pullup, now pullups enabled
--	--LED should be dimly lit
--	--set pin to pushpull
--	print("wotf LED PA_DDR:", dict.swim("WOTF", 0x5002, 0x04))	--PA2 is output CR1 set above makes pushpull
--	--LED is push/pull, ODR default to 0, so LED OFF
--	print("wotf LED PA_ODR:", dict.swim("WOTF", 0x5000, 0x04))	--PA2 output set LED ON!
--	print("wotf LED PA_ODR:", dict.swim("WOTF", 0x5000, 0x00))	--PA2 output set LED OFF!
--HIGH SPEED
--	print("wotf LED PA_CR1:", dict.swim("WOTF_HS", 0x5003, 0xFF))	--default is input w/o pullup, now pullups enabled
--	--LED should be dimly lit
--	--set pin to pushpull
--	print("wotf LED PA_DDR:", dict.swim("WOTF_HS", 0x5002, 0x04))	--PA2 is output CR1 set above makes pushpull
--	--LED is push/pull, ODR default to 0, so LED OFF
--	print("wotf LED PA_ODR:", dict.swim("WOTF_HS", 0x5000, 0x04))	--PA2 output set LED ON!
--	print("wotf LED PA_ODR:", dict.swim("WOTF_HS", 0x5000, 0x00))	--PA2 output set LED OFF!


	--holds SWIM pin low for 16usec+ to reset SWIM comms incase of error
--	dict.swim("SWIM_RESET")	

	--reset the chip, if bit2 set in CSR the SWIM exits active mode with this reset
--	print("wotf SRST:", dict.swim("SWIM_SRST"))	
	--SWIM is now inactive chip is executing it's program code

	--indicate to logic analyzer that test sequence above is complete
--	dict.pinport("CTL_SET_LO", "EXP0")
	dict.io("IO_RESET")	

end


local function start()

	dict.io("IO_RESET")	

	dict.io("SNES_INIT")	
	dict.io("SWIM_INIT", "SWIM_ON_EXP0")	
	dict.swim("SWIM_ACTIVATE")	

	--holds SWIM pin low for 16usec+ to reset SWIM comms incase of error
	--also verifies that device has SWIM active
      	dict.swim("SWIM_RESET")	

	--write 0A0h to SWIM_CSR
	--bit 5: allows entire memory range to be read & swim reset to be accessed
	--bit 7: masks internal reset sources (like WDT..?)
	cur_CSR = 0xA0
	if wotf(SWIM_CSR, cur_CSR) == "ACK" then
		print("Successfully established SWIM comms")
	else
		print("Unable to establish SWIM comms")
		return false
	end


	--read SWIM_CSR
      	--dict.swim("SWIM_RESET")	

	--print("wotf SRST:", dict.swim("SWIM_SRST"))	
	--print("wotf SWIM_CSR:", dict.swim("WOTF", 0x7F80, 0xA0))	

	--now the SRST command is available, whole memory range available, and internal resets disabled
	--by default there is now a breakpoint set at reset vector
	
	--reset the STM8 core
--	dict.swim("SWIM_SRST")
	system_reset()

	--the STM8 core is now stalled @ reset vector
	--can read/write to any address on STM8 core
	--if CIC ROP bit is set, we can only r/w to periph & SRAM

	--bit 2: SWIM is reset (exits active mode) when chip reset
	--this forces successful SWIM entry on each execution of script
--	cur_CSR = cur_CSR | 0x04
--	wotf(SWIM_CSR, cur_CSR)

	--print("switch to HS")
	--bit 4: SWIM HIGH SPEED (set for high speed)  SWIM RESET will set back to low speed
	--print("wotf SWIM_CSR:", dict.swim("WOTF", 0x7F80, 0xB4))	
	cur_CSR = cur_CSR | 0x10
	wotf(SWIM_CSR, cur_CSR)
	
--	swim_test()

	return true
end

local function printCSR()
	print(cur_CSR)
end

local function write_optn_bytes(debug)

	local toprint = nil--debug
	if debug then print("programming option bytes") end
	unlock_eeprom(true)

	--FLASH_CR2 and FLASH_NCR2 must be enabled to permit option byte writing
	--DEF_8BIT_REG_AT(FLASH_CR2,0x505b);  default 0x00
	--DEF_8BIT_REG_AT(FLASH_NCR2,0x505c); default 0xFF
	--BIT 7: OPT/NOPT
	wotf(0x505B, 0x80, true, toprint)
	wotf(0x505C, 0x7F, true, toprint)

	--need to enable AFR0 for TIM1 timer input pins
	--AFR0 Alternate function remapping option 0(2)
	--0: AFR0 remapping option inactive: Default alternate functions(1)
	--1: Port C5 alternate function = TIM2_CH1; port C6 alternate function =
	--TIM1_CH1; port C7 alternate function = TIM1_CH2.
	--0x4803 Alternate function remapping (AFR)
	--	 OPT2 AFR7 AFR6 AFR5 AFR4 AFR3 AFR2 AFR1 AFR0 0x00
	--0x4804 NOPT2 NAFR7 NAFR6 NAFR5 NAFR4 NAFR3 NAFR2 NAFR1 NAFR0 0xFF
	if debug then print("ENABLING AFR0 for TIM1") end
	wotf(0x4803, 0x01, true, toprint)
	wotf(0x4804, 0xFE, true, toprint)

	--disable option byte writing
	wotf(0x505B, 0x00, true, toprint)
	wotf(0x505C, 0xFF, true, toprint)

	lock_flash_eeprom(true)
	if debug then print("done with option byte programming") end
end

local function write_flash(file, debug)

	unlock_flash(true)

	local toprint = debug
	local buff_size = 1
	local byte_num = 0
	local readdata = 0
	local readresult = 0
	print("Programming STM8 CIC flash")
	for byte in file:lines(buff_size) do
		local data = string.unpack("B", byte, 1)
	--	print(data)
		wotf(0x8000+byte_num, data, true, toprint)
		--wotf(0x8000+byte_num, 0xFF, true, true)
		readresult, readdata = rotf(0x8000+byte_num, true, toprint )
		if readdata ~= data then
			print("ERROR flashing byte number", byte_num, "to STM8 CIC", data, readdata)
		end
		--if byte_num == 0x4C0 then
		--if byte_num == 0x020 then
		----if byte_num == 0x1FFF then
		--	return
		--end
		byte_num = byte_num + 1
	end

	print("Done with STM8 CIC flash")
	lock_flash_eeprom(true)
end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
swim.start = start
swim.write_flash = write_flash
swim.write_optn_bytes = write_optn_bytes
swim.printCSR = printCSR
swim.wotf = wotf
swim.rotf = rotf

-- return the module's table
return swim
