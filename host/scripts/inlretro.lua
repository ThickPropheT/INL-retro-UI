-- main script that runs application logic and flow



-- initial function called from C main
function main ()

	local dict = require "scripts.app.dict"
	local cart = require "scripts.app.cart"

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

	debug = true
	rv = cart.detect(debug)

end


main ()

