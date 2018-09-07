
-- create the module's table
local ciccom = {}

-- import required modules
local dict = require "scripts.app.dict"

-- file constants

-- local functions
local function start( debug )

	--CIC reset & clock needs to be low to start
	dict.pinport( "CTL_OP", "AFL")
	dict.pinport( "CTL_SET_LO", "AFL")	--CIC RESET LOW

	dict.pinport( "CTL_OP", "GBP")
	dict.pinport( "CTL_SET_LO", "GBP")	--INL CIC DATA LOW


--[[
	--reset the stm8 and disable SWIM, to get CIC in known state
	dict.io("SWIM_INIT", "SWIM_ON_A0")	
	if swim.start(true) then
		swim.stop_and_reset()
	else
		print("fail to activate SWIM")
	end

	--now the CIC should be restarting
	--takes ~0.5msec for it to get through boot sequence
	jtag.sleep(0.01) --10msec to be overly safe
	--after that time it's waiting for a CIC reset pulse to wake it
--]]



	--CIC CLK needs to be low before taking CIC RESET HIGH
	dict.pinport( "ADDR_ENABLE")
	dict.pinport( "ADDR_OP")
	dict.pinport( "ADDR_SET", 0)	--CIC CLK LOW (A0)

	--reset CIC by making it panic
--	dict.pinport( "CTL_SET_HI", "AFL")
--	dict.pinport( "CTL_SET_LO", "AFL")
--	rv = 2000	
--	while rv ~= 0 do 
--		dict.pinport( "ADDR_SET", 1)	--CIC CLK
--		dict.pinport( "ADDR_SET", 0)	--CIC CLK
--		rv = rv -1
--	end


	dict.pinport( "ADDR_SET", 0)	--CIC CLK
	dict.pinport( "CTL_SET_HI", "AFL")
	
	--CIC is now waiting for data to be clocked in using CIC CLK & KEY_DATA_OUT


	return
end

local function set_opcode(opcode)

	--KEY DATA IN is clocked in on rising edges of CIC CLK
	--"M" 0x4D (0b0100_1101 needs to be written to CIC to cause it to toggle mirroring
	--MSbit is latched first

	if opcode == "M" then
		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_HI", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_HI", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_HI", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	

	end

	--now that "M" is written to CIC, end OPCODE write sequence by taking CIC RESET LO
	dict.pinport( "CTL_SET_LO", "AFL")


end


local function write(data)

	--now send operand "V" (0x56) or "H" (0x48)

	if data == "H" then
		--HORIZONTAL
		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	

		dict.pinport( "CTL_SET_HI", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	

		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	

		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	

		dict.pinport( "CTL_SET_HI", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	

		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	

		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	

		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	

	elseif data == "V" then
	
		--VERTICAL
		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_HI", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_HI", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_HI", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	
		dict.pinport( "CTL_SET_LO", "GBP")
		dict.pinport( "ADDR_SET", 1)	
		dict.pinport( "ADDR_SET", 0)	
	end

end



-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
ciccom.start = start
ciccom.set_opcode = set_opcode
ciccom.write = write
ciccom.wotf = wotf
ciccom.rotf = rotf

-- return the module's table
return ciccom
