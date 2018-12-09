#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libusb.h>

//uncomment to DEBUG this file alone
#define DEBUG
//"make debug" to get DEBUG msgs on entire program
#include "dbg.h"
#include "usb_operations.h"

//lua libraries
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

// TODO: Migrate to argp for more descriptive flags.
const char *HELP =  "Usage: inlretro [options]\n\n"\
					"Options:\n"\
					"  -a [dumpram_filename]\tIf provided write ram to this filename\n"\
					"  -b [writeram_filename]If provided write this file's contents to ram\n"\
					"  -c [console]\t\tConsole port, {NES}\n"\
					"  -d [dump_filename]\tIf provided, dump cartridge ROMs to this filename\n"\
					"  -h\t\t\tDisplays this message.\n"\
					"  -m [mapper]\t\tNES console only, mapper ASIC on cartridge\n"\
					"  \t\t\t{mmc1,mmc3,nrom}\n"\
					"  -p [program_filename]\tIf provided, write this data to cartridge\n"\
					"  -s [lua_script]\tIf provided, use this script for main application logic\n"\
					"  -v [verify_filename]\tIf provided, writeback written rom to this filename\n"
					"  -w [wram_size_kb]\tNES-only, size of WRAM in kb\n"\
					"  -x [prg_rom_size_kb]\tNES-only, size of PRG-ROM in kb\n"\
					"  -y [chr_rom_size_kb]\tNES-only, size of CHR-ROM in kb\n";

// Struct used to control functionality.
typedef struct {
	char *console_name;
	char *mapper_name;
	int display_help;

	// NES Functionality
	int chr_rom_size_kb;
	int prg_rom_size_kb;
	int wram_size_kb;

	char *dump_filename;
	char *program_filename;
	char *ramdump_filename;
	char *ramwrite_filename;
	char *verify_filename;

	char *lua_filename;
} INLOptions;

// Returns true if given number is a power of 2, and at least minimum size.
int isValidROMSize(int x, int min) {
	return ((x & (x - 1)) == 0) && x >= min;
}


// Parse options and flags, create struct to drive program.
INLOptions* parseOptions(int argc, char *argv[]) {
	// lower case flags suggested for average user

	const char *FLAG_FORMAT = "a:b:hc:d:m:p:s:v:w:x:y:";
	int index = 0;
	int rv = 0;
	// opterr = 0;

	// Create options struct.
	INLOptions *opts = calloc(1, sizeof(INLOptions));
	opts->console_name = "";
	opts->ramdump_filename = "";
	opts->ramwrite_filename = "";
	opts->dump_filename = "";
	opts->mapper_name = "";
	opts->program_filename = "";
	opts->lua_filename = "";
	opts->verify_filename = "";

	//getopt returns args till done then returns -1
	//string of possible args : denotes 1 required additional arg
	//:: denotes optional additional arg follows	
	while((rv = getopt(argc, argv, FLAG_FORMAT)) != -1) {
		
		switch(rv) {

			case 'a': opts->ramdump_filename = optarg; break;
			case 'b': opts->ramwrite_filename = optarg; break;
			case 'h': opts->display_help = 1; break;
			case 'c': opts->console_name = optarg; break;
			case 'd': opts->dump_filename = optarg; break;
			case 'm': opts->mapper_name = optarg; break;
			case 'p': opts->program_filename = optarg; break;
			case 's': opts->lua_filename = optarg; break;
			case 'v': opts->verify_filename = optarg; break;
			case 'w': opts->wram_size_kb = atoi(optarg); break;
			case 'x': opts->prg_rom_size_kb = atoi(optarg); break;
			case 'y': opts->chr_rom_size_kb = atoi(optarg); break;
			case '?':
				if(
				   ( optopt == 'c' )
				|| ( optopt == 'd' )
				|| ( optopt == 'm' )
				|| ( optopt == 'p' )
				|| ( optopt == 's' )
				){
					log_err("Option -%c requires an argument.", optopt);
					return NULL;

				} else if ( isprint(optopt)) {
					log_err("Unknown option -%c .", optopt);
					return NULL;
				} else {
					log_err("Unknown option character '\\x%x'", optopt);
					return NULL;
				}
				log_err("Improper arguements passed");
					return NULL;
				break;
			default:
				printf("getopt failed to catch all arg cases");
				return 0;
		}
	
	}

	for( index = optind; index < argc; index++) {
		log_err("Non-option arguement: %s \n", argv[index]);
	}

	if (opts->display_help) {
		printf("%s", HELP);
		return NULL;
	}
	return opts;
}

void error_lua (lua_State *L, const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	vfprintf(stderr, fmt, argp);
	va_end(argp);
	lua_close(L);
	exit(EXIT_FAILURE);
}

int getglobint (lua_State *L, const char *var) {
	int isnum, result;
	lua_getglobal(L, var);
	result = (int)lua_tointegerx(L, -1, &isnum);
	if (!isnum)
		error_lua(L, "'%s' should be a number\n", var);
	lua_pop(L, 1); /* remove result from the stack */
	return result;
}

void load (lua_State *L, const char *fname, int *w, int *h) {
	if (luaL_loadfile(L, fname) || lua_pcall(L, 0, 0, 0))
		error_lua(L, "cannot run config. file: %s", lua_tostring(L, -1));
	*w = getglobint(L, "width");
	*h = getglobint(L, "height");
}

// Setup Lua environment.
lua_State *lua_init(INLOptions *opts) {
	lua_State *L = luaL_newstate(); //opens Lua
	luaL_openlibs(L); //opens the standard libraries

	// Register C functions that can be called from Lua.
	lua_pushcfunction(L, lua_usb_vend_xfr);
	lua_setglobal(L, "usb_vend_xfr");

	// Pass args to Lua
	// TODO: Do this in a less terrible way / don't register a million globals.
	lua_pushstring(L, opts->console_name);
	lua_setglobal(L, "console_name");

	lua_pushstring(L, opts->mapper_name);
	lua_setglobal(L, "mapper_name");

	lua_pushstring(L, opts->dump_filename);
	lua_setglobal(L, "dump_filename");

	lua_pushstring(L, opts->program_filename);
	lua_setglobal(L, "flash_filename");

	lua_pushstring(L, opts->verify_filename);
	lua_setglobal(L, "verify_filename");

	lua_pushstring(L, opts->ramdump_filename);
	lua_setglobal(L, "ramdump_filename");

	lua_pushstring(L, opts->ramwrite_filename);
	lua_setglobal(L, "ramwrite_filename");

	lua_pushinteger(L, opts->wram_size_kb);
	lua_setglobal(L, "nes_wram_size_kb");

	lua_pushinteger(L, opts->prg_rom_size_kb);
	lua_setglobal(L, "nes_prg_rom_size_kb");

	lua_pushinteger(L, opts->chr_rom_size_kb);
	lua_setglobal(L, "nes_chr_rom_size_kb");
	return L;
}

// Setup INL USB Device.
USBtransfer *usb_init_inldevice(libusb_context *context, int libusb_log) {
	// Create USBtransfer struct to hold all transfer info
	USBtransfer *transfer = calloc(1, sizeof(USBtransfer));	

	// Create USB device handle pointer to interact with retro-prog.
	transfer->handle = open_usb_device( context, libusb_log );

	return transfer;
}

// Close and cleanup INL USB Device.
void usb_close_inldevice(libusb_context* context, USBtransfer* transfer) {
	if (context && transfer) {
		close_usb(context, transfer->handle);
	}
	if (transfer) {
		free(transfer);
	}
}

// Safely cleanup for exiting program and release resources.
void cleanup(libusb_context *context, USBtransfer *transfer, lua_State *L) {
	usb_close_inldevice(context, transfer);
	if (L) {
		lua_close(L);
	}
}

int main(int argc, char *argv[]) 
{	
	// USB variables
	USBtransfer *transfer = NULL;
	// Context set to NULL since only acting as single user of libusb.
	libusb_context *context = NULL;
	
	// Default to no libusb logging.
	int libusb_log = LIBUSB_LOG_LEVEL_NONE;

	// Lua variables.
	lua_State *L = NULL;
	const char *LUA_SCRIPT_USB = "scripts/app/usb_device.lua";

	// Parse command-line options and flags.
	INLOptions *opts = parseOptions(argc, argv);
	if(!opts) {
		// If unparseable, exit.
		return 1;
	}

	// Check for sane user input.
	if (strcmp("NES", opts->console_name) == 0) {
		// ROM sizes must be non-zero, power of 2, and greater than 16.
		if (!isValidROMSize(opts->prg_rom_size_kb, 16)) {
			printf("PRG-ROM must be non-zero power of 2, 16kb or greater.\n");
			return 1;
		}
		// Not having CHR-ROM is normal for certain types of carts.
		// TODO: Update these checks with known info about mappers/carts.
		if (!isValidROMSize(opts->chr_rom_size_kb, 8) && opts->chr_rom_size_kb != 0) {
			printf("CHR-ROM must be zero or power of 2, 8kb or greater.\n");
			return 1;
		}

		// Not having WRAM is very normal.
		if (!isValidROMSize(opts->wram_size_kb, 8) && opts->wram_size_kb != 0) {
			printf("WRAM must be zero or power of 2, 8kb or greater.\n");
			return 1;
		}
	}

	// Start up Lua.
	L = lua_init(opts);
	
    // Setup and check connection to USB Device.
	// TODO get usb device settings from usb_device.lua

	// Lua script arg to set different libusb debugging options.
	check(!(luaL_loadfile(L, LUA_SCRIPT_USB) || lua_pcall(L, 0, 0, 0)),
	      "Cannot run config. file: %s", lua_tostring(L, -1));

	// Any value > 0 for libusb_log also prints debug statements in open_usb_device function.
	libusb_log = getglobint(L, "libusb_log");
	check(((libusb_log >= LIBUSB_LOG_LEVEL_NONE) && (libusb_log <= LIBUSB_LOG_LEVEL_DEBUG)), 
		  "Invalid LIBUSB_LOG_LEVEL: %d, must be from 0 to 4", libusb_log );

	transfer = usb_init_inldevice(context, libusb_log);
	check_mem(transfer);
	check(transfer->handle != NULL, "Unable to open INL retro-prog usb device handle.");

	// USB device is open, pass args and control over to Lua.
	// If lua_filename isn't set from args, use default script.
	char *DEFAULT_SCRIPT = "scripts/inlretro.lua";
	char *script = DEFAULT_SCRIPT;
	if (strlen(opts->lua_filename)) {
		script = opts->lua_filename;
	}
	check(!(luaL_loadfile(L, script) || lua_pcall(L, 0, 0, 0)),
		   "cannot run config. file: %s", lua_tostring(L, -1));
	
	// Program flow doesn't come back to this point until script call ends/returns.
	cleanup(context, transfer, L);
	return 0;

// 'check' macros goto this label if they fail.
error:
	printf("Fatal error encountered, exiting.\n");
	cleanup(context, transfer, L);
	return 1;
}