-- Main application flow for interacting with cartridges via USB device.
-- Refactored version that doesn't require commenting/uncommenting to change functionality.


-- Helper function that checks if a string is empty or nil.
local function isempty(s)
    return s == nil or s == ''
end


-- Wrapper for managing NES/Famicom operations.
function nes_exec(mapper, dump_filename, flash_filename, verify_filename)
    local dict = require "scripts.app.dict"
    local nes = require "scripts.app.nes"
    
    -- Always test!
    local do_test = true
    
    -- If a dump filename was provided, dump data from cartridge to a file.
    local do_read = not isempty(dump_filename)
    
    -- If a flash filename was provided, write its contents to the cartridge.
    local do_erase = not isempty(flash_filename)
    -- If writing, always erase.
    local do_program = do_erase
    
    -- If a verify_filename was provided, dump data from cartridge after flash to a file.
    local do_verify = not isempty(verify_filename)

    -- TODO: Add other mappers.
    local mappers = {
        mmc1 = require "scripts.nes.mmc1",
        mmc3 = require "scripts.nes.mmc3",
        nrom = require "scripts.nes.nrom",
        unrom = require "scripts.nes.unrom"
    }

    dict.io("IO_RESET")	
	dict.io("NES_INIT")	
    nes.detect_mapper_mirroring(true)
    
    m = mappers[mapper]
    if m == nil then
        print("UNSUPPORTED MAPPER")
    else
        -- Attempt requested operations with hardware!

        -- TODO: Do plumbing for interacting with RAM.
        m.process(do_test, do_read, do_erase, do_program, do_verify, dump_filename, flash_filename, verify_filename)
    end
end

-- Point of entry from C language half of program.
function main()

    -- Globals passed in from C: 
    --  console_name:    string, name of console.
    --  mapper_name:     string, name of mapper.
    --  dump_filename:   string, filename used for writing dumped data.
    --  flash_filename:  string, filename containing data to write cartridge.
    --  verify_filename: string, filename used for writing back data written to cartridge for verification.

    -- TODO: Add SNES support, as it appears to be currently usable?
    local consoles = {
        NES = nes_exec,
    }
    f = consoles[console_name]
    if f == nil then 
        print("UNSUPPORTED CONSOLE")
    else
        f(mapper_name, dump_filename, flash_filename, verify_filename)
    end
end

-- Don't do this. Next iteration will call a function, not the whole script.
main()
