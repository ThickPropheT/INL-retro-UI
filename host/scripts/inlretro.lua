-- main script that runs application logic and flow


USB_IN = 0x80	--device to host
USB_OUT = 0x00	--host to device

-- read all the C shared_dict*.h files and create tables with all values
-- This isn't 'Nam there are rules!
-- dictionary #define that start with underscore are skipped this skips over header and special cases
-- currently only finds lowercase #define statements (seems C does too!)
-- multiline comments /* comment */ are not permitted, will throw error!
-- #define statements must have a numeric value such as: #define FOO 4
-- #define without number will error like this: "#define FOO BAR" or "#define FOO BAR - 100" is bad too!
-- fills passed in table with keys and values to be used for making usb dictionary calls
-- accepts decimal or hex when 0x00 format

function create_dict_tables( table, file )
	assert(io.input(file, "r"))

	local count = 0
	local define = 0
	local define_end = 0
	local slash = 0
	local line 
	local comment
	for line in io.lines() do
		count = count + 1
		--search for multiline comment opening, starting at index 1, plain search = true
		if string.find( line, "/*", 1, true) then
			print("\n\n!!!ERROR!!!\nmultiline comment found line number", count)
			print("while parsing file:", file, "\nonly inline comments are permitted!!!\n")
			error("multiline comments not allowed in dictionary files!")
		end
		define, define_end = string.find( line, "#define") 
		if define then 
			slash = string.find(line, "//")
			--check for comment following define, if present cut out comment
			if slash and (slash>define) then
				--store comment contents for later parsing
				comment = string.sub(line, slash, -1)
				line = string.sub(line, 1, slash-1 )
			end
			--check for comment prior to define, skip if present
			if not (slash and (slash<define)) then 
				--print( count, define, line) 
				line = string.sub(line, define_end+1, -1)
				--match alpha and any trailing printable chars besides white space
				--this doesn't match guarded header/footer that starts with underscore
				local key = string.match( line, "%s%a%g+" )
				if key then
--	print ("\n\n",line)
					--key found, trim key from line
					local key_start, key_end
					key_start, key_end = string.find( line, key )
					line = string.sub( line, key_end+1, -1)
					--trim preceeding space needed to filter out underscore
					key = string.match( key, "%a%g+" )
					--trim any trailing underscore, trick that allows firmware to call macro with "same" macro
					if( (key:sub(-1,-1)) == '_' ) then
						key = key:sub(1,-2)
					end
					--match the number
					if string.match( line, "[^%s0-9a-fxA-F]+") then
			print("\n\n!!!ERROR!!!\ndictionary file #define parsing problem line:", count)
			print("while parsing file:", file, "\n", line, "is not a number!!!\n")
						error("dictionary #define statements must end with a number")
					end

					local number = string.match( line,  "%s+[0-9a-fxA-F]+")
					if number then
						number = string.match( number, "%s+[0-9a-fxA-F]+")
					end
--	print (number)
					--at this point we have the key and value, just convert from string
					number = tonumber( number )
--	print("key/val:", key, number)
--	print("key type:", type(key), "value type:", type(number))
					--add the key and value pair to the table
					table[key] = number

					--now process comment to find extra dictionary values stored in comments
--	print(comment)
					if comment then
						--return length "rlen" is set by RL=number (ie RL=10, RL=-0x100, etc)
						--positive RL denotes endpoint IN, negative denotes endpoint OUT
						--first match positive values
						local rlen = string.match( comment, "%s*RL%s*=%s*[0-9a-fA-Fx]+")
						if (rlen) then
							--trim RL= portion
							rlen = string.match( rlen, "[0-9a-fA-Fx]+")
							rlen = tonumber( rlen )
							--add return length to table
							table[key.."rlen"] = rlen;
						end
						--next check for negative values
						rlen = string.match( comment, "%s*RL%s*=%s*-%s*[0-9a-fA-Fx]+")
						if (rlen) then
							--trim RL=- portion
							rlen = string.match( rlen, "[0-9a-fA-Fx]+")
							rlen = - tonumber( rlen )
							--add return length to table
							table[key.."rlen"] = rlen;
						end
					end
--	print(key, ": val/rlen:", table[key], table[key.."rlen"], "\n\n")

				end
			end
		end
	end
end

--determine endpoint & wLength for usb transfer
--default is ep=IN, rlen=1 if different than those values it must be defined in dictionary
--positive values are associated with ep IN, negative ep OUT
function default_rlen_1_in( rlen )

	local ep
	if rlen then
		--RL defined in dictionary
		if rlen < 1 then
			ep = USB_OUT
			rlen = -rlen
		else
			ep = USB_IN
		end
	else	--RL not defined, assume default
		ep = USB_IN
		rlen = 1
	end

	return rlen, ep

end

function string_to_int( string, numbytes)

	local i = 0
	local rv = 0
	while i < numbytes do
		rv = rv | (string:byte(i+1) << 8*i)
		i = i+1
	end

	return rv

end

RETURN_ERR_IDX = 1
RETURN_LEN_IDX = 2
RETURN_DATA = 3


function dict_pinport( opcode, operand, misc, data )

	if not operand then operand = 0 end
	if not misc then misc = 0 end

	local wLength, ep = default_rlen_1_in(op_pinport[opcode.."rlen"])

	local count
	count, data = usb_vend_xfr( 
	--	ep,	dictionary		wValue[misc:opcode]     wIndex	wLength	 		data
		ep, dict["DICT_PINPORT"], ( misc<<8 | op_pinport[opcode]),	operand,	wLength,	data)
	--print(count)
	local error_code = data:byte(RETURN_ERR_IDX)
	local data_len =   data:byte(RETURN_LEN_IDX)
	--print("error:", error_code, "data_len:",  data_len)
	
	if error_code ~= err_codes["SUCCESS"] then
		print("ERROR!!! device error code:", error_code)
	end

	if data_len and data_len ~= (wLength - RETURN_LEN_IDX) then
		print("WARNING!! Device's return data length:", data_len, "did not match expected:", wLength-RETURN_LEN_IDX)
	end

	--process the return data string and return it to calling function
	if data_len then
		return string_to_int( data:sub(RETURN_DATA, data_len+RETURN_DATA), data_len) 
	else 
		return nil
	end 


end

-- initial function called from C main
function main ()
	--check that dictionary files were found and parsed
	--print(op_buffer)
	--if not ( dict and op_pinport and op_buffer and op_io and op_operation and op_nes and op_snes ) then
	--	print("ERROR: couldn't find/parse dictionary files in shared folder!")
	--end

	--			     print	dictionary		opcode               addr   miscdata endpt buffer length
	--print("dict_call ", dict_call( 1,	dict["DICT_PINPORT"],	op_pinport["LED_OFF"],	0,	0,	USB_IN,	nil,	8), "\n")
	--
	print("return len", op_pinport["LED_ONrlen"])
	local rv = dict_pinport( "LED_ON" )
	if rv then
		print(string.format("%X", rv))
	end

	--[[{i=0
	while ( i < 8192 )  do
		usb_vend_xfr( USB_IN, dict["DICT_PINPORT"], op_pinport["LED_ON"],	0,	128,	"wowdata!!!!")
		i = i+1
	end
	--]]
end

-- Dictionary table definitions
dict = {}
op_pinport = {}
op_buffer = {}
op_io = {}
op_operation = {}
op_nes = {}
op_snes = {}
err_codes = {}

-- Dictionary table definitions created by calling parser
-- call functions desired to run when script is called
create_dict_tables( dict, 	"../shared/shared_dictionaries.h")
create_dict_tables( op_pinport, "../shared/shared_dict_pinport.h")
create_dict_tables( op_buffer, 	"../shared/shared_dict_buffer.h")
create_dict_tables( op_io,  	"../shared/shared_dict_io.h")
create_dict_tables( op_operation,  "../shared/shared_dict_operation.h")
create_dict_tables( op_nes,  	"../shared/shared_dict_nes.h")
create_dict_tables( op_snes,  	"../shared/shared_dict_snes.h")
create_dict_tables( err_codes, 	"../shared/shared_errors.h")

main ()

