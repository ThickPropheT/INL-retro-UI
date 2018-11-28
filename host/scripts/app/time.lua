
-- create the module's table
local time = {}

-- import required modules

-- file constants & variables
local tstart


-- local functions
local function start()
	tstart = os.clock()
end


--send the number of KBytes flashed so it can report KBps
local function report(sizeKB)
	local time = os.clock() - tstart
	print("total time:", time, "seconds, average speed:", (sizeKB/time), "KBps")
end


-- global variables so other modules can use them


-- call functions desired to run when script is called/imported


-- functions other modules are able to call
time.start = start
time.report = report

-- return the module's table
return time
