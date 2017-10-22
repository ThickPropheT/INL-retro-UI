
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

local function stm8_system_reset()
	--if cur_CSR has bit 2 set, SWIM must be reactivated 
end

local function reset_swim()
	print("resetting SWIM")
      	dict.swim("SWIM_RESET")	

--	wotf(SWIM_CSR, cur_CSR)
	--must rewrite current value of SWIM_CSR register as HIGHSPEED is cleared during SWIM RESET
      	dict.swim("WOTF", SWIM_CSR, cur_CSR)	
end

local function rotf(addr, hspeed, num)

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
		print("rotf", string.format(" %X: %X, result ", addr, data), result)
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
end

local function wotf(addr, data, hspeed, num)

	local result = ECODE.NAK
	local tries = 5
	local resets = 3

	local opcode = "WOTF"
	if hspeed then
		opcode = "WOTF_HS"
	end

	while result ~= "ACK" and tries > 0 do
		result = dict.swim(opcode, addr, data)
		result = get_key_for_value( ECODE, result)
		print("wotf", string.format(" %X: %X, result ", addr, data), result)
		tries = tries - 1
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
	end
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
	--print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
--	print("wotf SWIM_CSR:", dict.swim("WOTF", 0x7F80, 0xA0))	
	cur_CSR = 0xA0
	wotf(SWIM_CSR, cur_CSR)
	--print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))

	--read SWIM_CSR
      	--dict.swim("SWIM_RESET")	

	--print("wotf SRST:", dict.swim("SWIM_SRST"))	
	--print("wotf SWIM_CSR:", dict.swim("WOTF", 0x7F80, 0xA0))	

	--now the SRST command is available, whole memory range available, and internal resets disabled
	--by default there is now a breakpoint set at reset vector
	
	--reset the STM8 core
	print("wotf SRST:", dict.swim("SWIM_SRST"))	

	--the STM8 core is now stalled @ reset vector
	--can read/write to any address on STM8 core
	--if CIC ROP bit is set, we can only r/w to periph & SRAM

	--bit 2: SWIM is reset (exits active mode) when chip reset
	--this forces successful SWIM entry on each execution of script
--	print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
	--print("wotf SWIM_CSR:", dict.swim("WOTF", 0x7F80, 0xA4))	
--	cur_CSR = cur_CSR | 0x04
--	wotf(SWIM_CSR, cur_CSR)
--	print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))

	wotf(0x0000, 0x00) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	rotf(0x0000) 
	print("switch to HS")
	--bit 4: SWIM HIGH SPEED (set for high speed)  SWIM RESET will set back to low speed
	--print("wotf SWIM_CSR:", dict.swim("WOTF", 0x7F80, 0xB4))	
	cur_CSR = cur_CSR | 0x10
	wotf(SWIM_CSR, cur_CSR)
--	print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF_HS", 0x7F80)))
	--print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF_HS", 0x7F80)))

	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8028)))
	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8029)))
	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802A)))
	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802B)))

	--print("wotf :", dict.swim("WOTF_HS", 0x8028, 0x49))
	--print("wotf :", dict.swim("WOTF_HS", 0x8029, 0x4E))
	--print("wotf :", dict.swim("WOTF_HS", 0x802A, 0x4C))
	--print("wotf :", dict.swim("WOTF_HS", 0x802B, 0x20))

	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8028)))
	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8029)))
	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802A)))
	--print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802B)))

--read then write to SRAM
--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x0000)))

      	--print("wotf :", dict.swim("WOTF_HS", 0x0000, 0x00))
	--high speed now, enable flag with true
	wotf(0x0000, 0x00, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	rotf(0x0000, true) 
	wotf(0x0000, 0xEE, true) 
	rotf(0x0000, true) 
	wotf(0x0000, 0xAA, true) 
	rotf(0x0000, true) 
	wotf(0x0000, 0x55, true) 
	rotf(0x0000, true) 

--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x0000)))

--read then write to eeprom
--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x4000)))
--
--      	--need to unlock the eeprom first!
--	--Write 0xAE then 56h in
--	--FLASH_DUKR (0x00 5064)(1)(2)
--      	print("wotf :", dict.swim("WOTF_HS", 0x5064, 0xAE))
--      	print("wotf :", dict.swim("WOTF_HS", 0x5064, 0x56))
--	--write data
--      	print("wotf :", dict.swim("WOTF_HS", 0x4000, 0x00))
--
--	--lock eeprom
--	--Reset bit 3 (DUL)
--	--in FLASH_IAPSR (0x00 505F)
--      	print("wotf :", dict.swim("WOTF_HS", 0x505F, 0x00))
--
--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x4000)))

--read then write to flash
--	print("rotf :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8028)))

	--need to unlock the flash first!
	--Write 0x56 then 0xAE in
	--FLASH_PUKR (0x00 5062)(3)
--      	print("wotf :", dict.swim("WOTF_HS", 0x5062, 0x56))
--      	print("wotf :", dict.swim("WOTF_HS", 0x5062, 0xAE))
--	--write data
--	byte = 0x8028
--	data = 0x12
--	while byte < 0x8030 do
--		while (dict.swim("WOTF_HS", byte, data) ~= 1) do
--			print("woft nak ", byte)
--		end
--	
--		byte = byte + 1
--		data = data + 0x11
--
--	end

--      	print("wotf 28:", dict.swim("WOTF_HS", 0x8028, 0xEE))
--      	print("wotf 29:", dict.swim("WOTF_HS", 0x8029, 0x11))
--      	print("wotf 2A:", dict.swim("WOTF_HS", 0x802A, 0x22))
--      	print("wotf 2B:", dict.swim("WOTF_HS", 0x802B, 0x33))
--      	print("wotf 2C:", dict.swim("WOTF_HS", 0x802C, 0x44))
--      	print("wotf 2D:", dict.swim("WOTF_HS", 0x802D, 0x55))
--      	print("wotf 2E:", dict.swim("WOTF_HS", 0x802E, 0x66))
--      	print("wotf 2F:", dict.swim("WOTF_HS", 0x802F, 0x77))

	--lock flash 
	--Reset bit 1 (PUL)
--	--in FLASH_IAPSR (0x00 505F)
--      	print("wotf :", dict.swim("WOTF_HS", 0x505F, 0x00))
--
--	print("rotf 28 :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8028)))
--	print("rotf 29 :", string.format("%X  %X", dict.swim("ROTF_HS", 0x8029)))
--	print("rotf 2A :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802A)))
--	print("rotf 2B :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802B)))
--	print("rotf 2C :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802C)))
--	print("rotf 2D :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802D)))
--	print("rotf 2E :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802E)))
--	print("rotf 2F :", string.format("%X  %X", dict.swim("ROTF_HS", 0x802F)))
--
--
--	--print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
	--print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
	--print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
	--print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))
	--print("rotf SWIM_CSR:", string.format("%X  %X", dict.swim("ROTF", 0x7F80)))

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
	print("wotf LED PA_CR1:", dict.swim("WOTF_HS", 0x5003, 0xFF))	--default is input w/o pullup, now pullups enabled
	--LED should be dimly lit
	--set pin to pushpull
	print("wotf LED PA_DDR:", dict.swim("WOTF_HS", 0x5002, 0x04))	--PA2 is output CR1 set above makes pushpull
	--LED is push/pull, ODR default to 0, so LED OFF
	print("wotf LED PA_ODR:", dict.swim("WOTF_HS", 0x5000, 0x04))	--PA2 output set LED ON!
	print("wotf LED PA_ODR:", dict.swim("WOTF_HS", 0x5000, 0x00))	--PA2 output set LED OFF!


	--holds SWIM pin low for 16usec+ to reset SWIM comms incase of error
--	dict.swim("SWIM_RESET")	

	--reset the chip, if bit2 set in CSR the SWIM exits active mode with this reset
--	print("wotf SRST:", dict.swim("SWIM_SRST"))	
	--SWIM is now inactive chip is executing it's program code

	--indicate to logic analyzer that test sequence above is complete
--	dict.pinport("CTL_SET_LO", "EXP0")
	dict.io("IO_RESET")	

end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
swim.start = start
swim.wotf = wotf
swim.rotf = rotf

-- return the module's table
return swim
