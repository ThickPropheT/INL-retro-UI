
-- create the module's table
local jtag = {}

-- import required modules
local dict = require "scripts.app.dict"

-- file constants

-- local functions
local function wait_pbje_done( num_polls, debug )

	local status

	while( num_polls > 0 ) do

		status = dict.jtag("GET_STATUS") 
		if( status == op_jtag["PBJE_DONE"]) then
			if( debug) then print("PBJE done num polls left:", num_polls) end
			return true
		else
			if( debug) then print("PBJE wasn't done, status:", status) end
		end
		num_polls = num_polls - 1
	end

	print("JTAG timed out while waiting for PBJE_DONE")
	return false

end


local function run_jtag( debug )

	dict.io("JTAG_INIT", "JTAG_ON_EXP0_3")
	--dict.jtag("SET_3B_DATA", 0x0201, 0x03 )
	--print("return data:", dict.jtag("GET_6B_DATA"))
	--print(dict.jtag("SET_CMD", "PBJE_STATE_CHG"))
	--print(dict.jtag("GET_CMD"))
	--print(dict.jtag("GET_STATUS"))

	--first put/verify jtag statemachine is in RESET
	dict.jtag("SET_2B_DATA", 0xFFFF)
	dict.jtag("SET_NUMCLK", 8)
	rv = dict.jtag("SET_CMD_WAIT", "PBJE_STATE_CHG")
	--verify command was done
	if(rv ~= op_jtag["PBJE_DONE"]) then print("error JTAG not done, status: ", rv) end


	--by default jtag should be in IDCODE or BYPASS if IDCODE not present
	
	--change to SCAN-DR state
	--reset-DRshift c4 m0010
	dict.jtag("SET_2B_DATA", 0x0002)
	dict.jtag("SET_NUMCLK", 4)
	rv = dict.jtag("SET_CMD_WAIT", "PBJE_STATE_CHG")
	--verify command was done
	if(rv ~= op_jtag["PBJE_DONE"]) then print("error JTAG not done, status: ", rv) end

	--scan out 32bit IDCODE while scanning in 1's to TDI
	dict.jtag("SET_NUMCLK", 32)
	dict.jtag("SET_CMD", "PBJE_TDO_SCAN1")
	--verify done before updating PBJE values
	jtag.wait_pbje_done( 4, true )

	rv = dict.jtag("GET_6B_DATA")
	print("return data:", string.format(" %X, ",rv))
	if( rv == 0x1281043 ) then
	-- Mach XO 256   01281043
	-- 4032v	(01805043)
	-- 4064v	(01809043)
	--
	-- 9536xl
	-- //Loading device with 'idcode' instruction.
	-- SIR 8 TDI (fe) SMASK (ff) ;
	-- SDR 32 TDI (00000000) SMASK (ffffffff) TDO (f9602093) MASK (0fffffff) ;
	--
	-- 9572xl
	-- //Loading device with 'idcode' instruction.
	-- SIR 8 TDI (fe) SMASK (ff) ;
	-- SDR 32 TDI (00000000) SMASK (ffffffff) TDO (f9604093) MASK (0fffffff) ;
	-- test read gives 59604093
		print("IDCODE matches MACHXO-256")
	else
		print("no match for IDCODE")
	end
	
	--Mach XO verify ID code
--	! Check the IDCODE
--
--	! Shift in IDCODE(0x16) instruction
--	SIR     8       TDI  (16);
--	SDR     32      TDI  (FFFFFFFF)
--	                TDO  (01281043)
--	                MASK (FFFFFFFF);
	
--	--change to SCAN-IR state
--	dict.jtag("SET_2B_DATA", 0x0006)
--	dict.jtag("SET_NUMCLK", 5)
--	dict.jtag("SET_CMD", "PBJE_STATE_CHG")
--	--verify done before updating PBJE values
--	jtag.wait_pbje_done( 4, true )
--
--	--scan in IDCODE instruction
--	dict.jtag("SET_2B_DATA", 0x0016)
--	dict.jtag("SET_NUMCLK", 8)
--	dict.jtag("SET_CMD", "PBJE_TDI_SCAN")
--	--verify done before updating PBJE values
--	jtag.wait_pbje_done( 4, true )
--
--	--change to SCAN-DR state
--	--shift-pause c2 m01
--	--IRpause-DRshift c5 m00111
--	--together c7 m001_1101 -> 0x1D
--	dict.jtag("SET_2B_DATA", 0x001D)
--	dict.jtag("SET_NUMCLK", 7)
--	dict.jtag("SET_CMD", "PBJE_STATE_CHG")
--	--verify done before updating PBJE values
--	jtag.wait_pbje_done( 4, true )
--
--
--	--scan out 32bit IDCODE while scanning in 1's to TDI
--	dict.jtag("SET_NUMCLK", 32)
--	dict.jtag("SET_CMD", "PBJE_TDO_SCAN1")
--	--verify done before updating PBJE values
--	jtag.wait_pbje_done( 4, true )
--
--	print("return data:", dict.jtag("GET_6B_DATA"))

end

-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
jtag.wait_pbje_done = wait_pbje_done
jtag.run_jtag = run_jtag

-- return the module's table
return jtag
