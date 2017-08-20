
-- create the module's table
local dump = {}

-- import required modules
local dict = require "scripts.app.dict"
local buffers = require "scripts.app.buffers"

-- file constants

-- local functions


--//main collected as much data about cart as possible without reading roms
--//now it's time to start running CRC's to try and finalize mapper/config
--//Once final mapper is known store header data in rom file and start dumping!
local function dump_nes( file, debug )

	local buff0 = 0
	local buff1 = 1
	--local i = nil
	local cur_buff_status = 0
--	uint8_t data[buff_size];
	local data = nil --lua stores data in strings

	if debug then print("dumping cart") end
--
--	//TODO provide user arg to force all these checks passed
--	//first check if any provided args differ from what was detected
--	check( (cart->console != UNKNOWN), "cartridge not detected, must provide console if autodetection is off");
--
--	if ( rom->console != UNKNOWN ) {
--		check( rom->console == cart->console, 
--			"request system dump doesn't match detected cartridge");
--	}
--	if ( (cart->mapper != UNKNOWN) && (rom->mapper != UNKNOWN) ) {
--		check( rom->mapper == cart->mapper,	
--			"request mapper dump doesn't match detected mapper");
--	}
--
--	//start with reset and init
	dict.io("IO_RESET")
	dict.io("NES_INIT")
--	//Run some CRC's to determine size of memories
--
--	//setup buffers and manager
	dict.operation("SET_OPERATION", op_buffer["RESET"] )
--	//reset buffers first
	dict.buffer("RAW_BUFFER_RESET")

--	//need to allocate some buffers for dumping
--	//2x 128Byte buffers
	local num_buffers = 2
	local buff_size = 128	
	print("allocating buffers")
	assert(buffers.allocate( num_buffers, buff_size ), "fail to allocate buffers")

--	//set buffer elements as needed
--	//set reload which gets added to page_num after each buffer read
--	//set reload to 256 = 1 when translated to page_num (done in allocate buffers funct)
--	//set page_num to non-zero if offset arg sent
--	//set mem_type and part_num to designate how to get/write data
--	check(! set_mem_n_part( transfer, buff0, PRGROM, MASKROM ), "Unable to set mem_type and part");
	print("setting map n part")
	dict.buffer("SET_MEM_N_PART", (op_buffer["PRGROM"]<<8 | op_buffer["MASKROM"]), buff0 )
	dict.buffer("SET_MEM_N_PART", (op_buffer["PRGROM"]<<8 | op_buffer["MASKROM"]), buff1 )
--	//set multiple and add_mult only when flashing
--	//set mapper, map_var, and function to designate read/write algo
--
--	//just dump visible NROM memory to start
	print("setting map n mapvar")
	dict.buffer("SET_MAP_N_MAPVAR", (op_buffer["NROM"]<<8 | op_buffer["NOVAR"]), buff0 )
	dict.buffer("SET_MAP_N_MAPVAR", (op_buffer["NROM"]<<8 | op_buffer["NOVAR"]), buff1 )
--
--	//tell buffers what function to use for dumping
--	//TODO when start implementing other mappers
--
--	//debugging print out buffer elements
--	//get_operation( transfer );
	--print("\nget operation:")
	--dict.operation("GET_OPERATION" )
	--print("\n\ngetting cur_buff status")
	--dict.buffer("GET_CUR_BUFF_STATUS" )
	--print("\n\ngetting elements")
	--dict.buffer("GET_PRI_ELEMENTS", nil, buff0 )
	--dict.buffer("GET_PRI_ELEMENTS", nil, buff1 )
	--dict.buffer("GET_SEC_ELEMENTS", nil, buff0 )
	--dict.buffer("GET_SEC_ELEMENTS", nil, buff1 )
	--dict.buffer("GET_PAGE_NUM", nil, buff0 )
	--dict.buffer("GET_PAGE_NUM", nil, buff1 )
--
	print("\n\nsetting operation STARTDUMP");
--	//inform buffer manager to start dumping operation now that buffers are initialized
--	check(! set_operation( transfer, STARTDUMP ), "Unable to set buffer operation");
	dict.operation("SET_OPERATION", op_buffer["STARTDUMP"] )
--
	--need these calls to delay things a bit to let first buffer dump complete..
--	dict.operation("GET_OPERATION" )
--	dict.buffer("GET_PRI_ELEMENTS", nil, buff0 )
--	dict.buffer("GET_PRI_ELEMENTS", nil, buff1 )
--	//manager updates buffer status' so they'll start dumping
--	//once they're full manager prepares them to be read back on USB payloads
--	//once the next payload request happens manager knows last buffer can start dumping again
--	//buffer updates it's elements and goes off to dump next page
--
	print("starting first payload");
	--dict.buffer("BUFF_PAYLOAD")
	--file:write("HELLO FILE!")
--	file:write( dict.buffer_payload_in( buff_size ))
--	print("first payload done");
--
--	print("second payload");
--	file:write( dict.buffer_payload_in( buff_size ))
--
--	clock_t tstart, tstop;
--	tstart = clock();
--
	--wait for first buffer to finish dumping before calling payload
	buffers.status_wait({buff0}, {"DUMPED"}) 
--	//now just need to call series of payload IN transfers to retrieve data
	--for i=1, (1024*1024/buff_size) do
	for i=1, (32*1024/buff_size) do

--		//ensure cur_buff is DUMPED prior to requsting data
--		Removed this check as the device USB driver & buffer.c should properly handle this case
--		if the device isn't ready it should ignore the host and make it resend it's request.
--		check(! get_cur_buff_status( transfer, &cur_buff_status ), "Error retrieving cur_buff->status");
		--cur_buff_status = dict.buffer("GET_CUR_BUFF_STATUS")
		--while (cur_buff_status ~= op_buffer["DUMPED"]) do
--		--while (cur_buff_status != DUMPED ) {
		--	print("cur_buff->status: ", cur_buff_status)
		--	cur_buff_status = dict.buffer("GET_CUR_BUFF_STATUS")
--		--	check(! get_cur_buff_status( transfer, &cur_buff_status ), "Error retrieving cur_buff->status");
--		--}
		--end
--	//for( i=0; i<(8*KByte/buff_size); i++) {
--		//payload transfer in and append to file
--	//	if ( i % 256 == 0 ) debug("payload in #%d", i);
--		check(! payload_in( transfer, data, buff_size ), "Error with payload IN");
--		if (i==0) printf("first byte: %x\n", data[0]);
--		check(! append_to_file( rom, data, buff_size ), "Error with file append");
		file:write( dict.buffer_payload_in( buff_size ))
	end
--	}
	print("payload done");
	--buffer manager updates from USB_UNLOADING -> DUMPING -> DUMPED
	--while one buffer is unloading, it sends next buffer off to dump
	--payout opcode updates from DUMPED -> USB_LOADING
	--so end result is buff0 will get sent off to dump extra data that's not needed
	--but we never call payload on it, so buff1 never moves from USB_UNLOADING
	--but it has to be done unloading if we sent a subsequent setup packet
	--buffers.status_wait({buff0, buff1}, {"DUMPED","USB_UNLOADING"}) 
	--in reality I don't think this wait is needed.  Because we've already gotten our data if
	--we're at this point..
	--what we really need to do is set the buffer's status' to a reset state 
	--that way we can give them new instructions before restarting DUMP of CHR portion
	--best way can think of doing this is a operation "RESET" which updates buffer status,
	--without deallocating them.  In reality should be able to do this by setting operation to reset.
	--and having STARTDUMP/FLASH initialize buffer status' as well.
	dict.operation("SET_OPERATION", op_buffer["RESET"] )
	dict.buffer("RAW_BUFFER_RESET")

--	//need to allocate some buffers for dumping
--	//2x 128Byte buffers
	num_buffers = 2
	buff_size = 128	
	print("allocating buffers")
	assert(buffers.allocate( num_buffers, buff_size ), "fail to allocate buffers")

--	//set buffer elements as needed
--	//set reload which gets added to page_num after each buffer read
--	//set reload to 256 = 1 when translated to page_num (done in allocate buffers funct)
--	//set page_num to non-zero if offset arg sent
--	//set mem_type and part_num to designate how to get/write data
--	check(! set_mem_n_part( transfer, buff0, PRGROM, MASKROM ), "Unable to set mem_type and part");
	print("setting map n part")
	dict.buffer("SET_MEM_N_PART", (op_buffer["CHRROM"]<<8 | op_buffer["MASKROM"]), buff0 )
	dict.buffer("SET_MEM_N_PART", (op_buffer["CHRROM"]<<8 | op_buffer["MASKROM"]), buff1 )
--	//set multiple and add_mult only when flashing
--	//set mapper, map_var, and function to designate read/write algo
--
--	//just dump visible NROM memory to start
	print("setting map n mapvar")
	dict.buffer("SET_MAP_N_MAPVAR", (op_buffer["NROM"]<<8 | op_buffer["NOVAR"]), buff0 )
	dict.buffer("SET_MAP_N_MAPVAR", (op_buffer["NROM"]<<8 | op_buffer["NOVAR"]), buff1 )
--
--
	print("\n\nsetting operation STARTDUMP");
--	//inform buffer manager to start dumping operation now that buffers are initialized
--	check(! set_operation( transfer, STARTDUMP ), "Unable to set buffer operation");
	dict.operation("SET_OPERATION", op_buffer["STARTDUMP"] )
--
--	tstop = clock();
--	float timediff = ( (float)(tstop-tstart) / CLOCKS_PER_SEC);
--	printf("total time: %fsec, speed: %fKBps", timediff, (512/timediff));
--	//TODO flush file from time to time..?
--
--
--	//tell buffer manager when to stop
--	// or not..?  just reset buffers and start next memory or quit
--	//reset buffers and setup to dump CHR-ROM
	--Can't get things to work without resetting all the buffers for some reason..
	--Think the issue is need to get the buffer status needs reset
--	dict.buffer("RAW_BUFFER_RESET")
--	print("reallocate buffers")
--	assert(buffers.allocate( num_buffers, buff_size ), "fail to allocate buffers")
	
	--wait for first buffer to finish dumping prior to calling payload
	buffers.status_wait({buff0}, {"DUMPED"}) 
	
	for i=1, (8*1024/buff_size) do
		--cur_buff_status = dict.buffer("GET_CUR_BUFF_STATUS")
		--while (cur_buff_status ~= op_buffer["DUMPED"]) do
--		--while (cur_buff_status != DUMPED ) {
		--	print("cur_buff->status: ", cur_buff_status)
		--	cur_buff_status = dict.buffer("GET_CUR_BUFF_STATUS")
--		--	check(! get_cur_buff_status( transfer, &cur_buff_status ), "Error retrieving cur_buff->status");
--		--}
		--end
		file:write( dict.buffer_payload_in( buff_size ))
	end
	print("payload done");
--
--	//close file in main
--
--	//reset io at end
	dict.operation("SET_OPERATION", op_buffer["RESET"] )
	dict.buffer("RAW_BUFFER_RESET")
	dict.io("IO_RESET")

	return true
end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
dump.dump_nes = dump_nes

-- return the module's table
return dump
