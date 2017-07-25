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
	local comment = 0
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
			comment = string.find(line, "//")
			--check for comment following define, if present cut out comment
			if comment and (comment>define) then
				line = string.sub(line, 1, comment-1 )
			end
			--check for comment prior to define, skip if present
			if not (comment and (comment<define)) then 
				--print( count, define, line) 
				line = string.sub(line, define_end+1, -1)
				--match alpha and any trailing printable chars besides white space
				--this doesn't match guarded header/footer that starts with underscore
				local key = string.match( line, "%s%a%g+" )
				if key then
	--print ("\n\n",line)
					--key found, trim key from line
					local key_start, key_end
					key_start, key_end = string.find( line, key )
					line = string.sub( line, key_end+1, -1)
					--trim preceeding space needed to filter out underscore
					key = string.match( key, "%a%g+" )
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
	--print (number)
					--at this point we have the key and value, just convert from string
					number = tonumber( number )
	--print("key/val:", key, number)
	--print("key type:", type(key), "value type:", type(number))
					--add the key and value pair to the table
					table[key] = number
				end
			end
		end
	end
end


-- initial function called from C main
function main ()
	--			     print	dictionary		opcode               addr   miscdata endpt buffer length
	print("dict_call ", dict_call( 1,	dict["DICT_PINPORT"],	op_pinport["LED_OFF"],	0,	0,	USB_IN,	nil,	8), "\n")
end

-- Dictionary table definitions
dict = {}
op_pinport = {}
op_buffer = {}
op_io = {}
op_operation = {}
op_nes = {}
op_snes = {}

-- call functions desired to run when script is called
create_dict_tables( dict, 	"../shared/shared_dictionaries.h")
create_dict_tables( op_pinport, "../shared/shared_dict_pinport.h")
create_dict_tables( op_buffer, 	"../shared/shared_dict_buffer.h")
create_dict_tables( op_io,  	"../shared/shared_dict_io.h")
create_dict_tables( op_operation,  "../shared/shared_dict_operation.h")
create_dict_tables( op_nes,  	"../shared/shared_dict_nes.h")
create_dict_tables( op_snes,  	"../shared/shared_dict_snes.h")

main ()

