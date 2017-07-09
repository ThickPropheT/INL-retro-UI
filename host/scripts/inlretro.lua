-- main script that runs application logic and flow

-- Dictionary table definitions
dict = {}
dict["DICT_PINPORT"] = 1
dict["DICT_IO"] = 2
dict["DICT_NES"] = 3
dict["DICT_SNES"] = 4
dict["DICT_BUFFER"] = 5
dict["DICT_USB"] = 6
dict["DICT_OPER"] = 7

pinport = {}
pinport["LED_IP"] = 65
pinport["LED_OP"] = 66
pinport["LED_OFF"] = 67
pinport["LED_ON"] = 68

USB_IN = 0x80	--device to host
USB_OUT = 0x00	--host to device


-- initial function called from C main
function main ()
	--			     print	dictionary		opcode               addr   miscdata endpt buffer length
	print("dict_call ", dict_call( 1,	dict["DICT_PINPORT"],	pinport["LED_OFF"],	0,	0,	USB_IN,	nil,	1), "\n")
end

-- call functions desired to run when script is called
main ()

