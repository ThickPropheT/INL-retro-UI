#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <libusb.h>

//uncomment to DEBUG this file alone
#define DEBUG
//"make debug" to get DEBUG msgs on entire program
#include "dbg.h"

//#include "shared_dictionaries.h"
#include "usb_operations.h"
//#include "dictionary.h"
//#include "write_operations.h"
//#include "erase.h"
//#include "test.h"
//#include "cartridge.h"
//#include "file.h"
//#include "dump.h"
//#include "flash.h"
//#include "shared_enums.h"

//lua libraries
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

// Struct used to control functionality.
typedef struct {
	char *console_name;
	char *mapper_name;
	int display_help;

	char *dump_filename;
	char *program_filename;
	char *lua_filename;
} INLOptions;

// Parse options and flags, create struct to drive program.
INLOptions* parseOptions(int argc, char *argv[]) {
	// lower case flags suggested for average user

	const char *FLAG_FORMAT = "hc:d:m:p:s:";
	int index = 0;
	int rv = 0;
	// opterr = 0;
	/* FLAGS NOT YET SUPPORTED
	int e_flag = 0; //FORCE ERASE
	int f_flag = 0; //FORCE ALL CHECKS PASSING
	int i_flag = 0; //input file is interlaced
	int n_flag = 0;	//LED OFF
	int o_flag = 0;	//LED ON
	int t_flag = 0; //test all SRAMs found
	int x_flag = 0; //disable all auto detecting
	int y_flag = 0; //diable all auto doubling

	char *b_value = NULL; //SUBMAPPER #
	char *p_value = NULL; //PROGRAM FILE
	char *v_value = NULL; //MAPPER VARIANT

	//upper case flags suggested for ADVANCED users
	int T_flag = 0; //TEST

	char *C_value = NULL; //program chr file
	char *L_value = NULL; //LIBUSB debugging value
	char *K_value = NULL; //connect to kazzo firmware version #
	char *O_value = NULL; //start read/writing at offset base value
	char *P_value = NULL; //program prg file
	char *S_value = NULL; //program SNES binary file
	char *W_value = NULL; //program WRAM/SRAM file
	*/
	INLOptions *opts = calloc(1, sizeof(INLOptions));
	
	//getopt returns args till done then returns -1
	//string of possible args : denotes 1 required additional arg
	//:: denotes optional additional arg follows	
	while((rv = getopt(argc, argv, FLAG_FORMAT)) != -1) {
		
		switch(rv) {
			// case 'o': o_flag = 1; break;
			// case 'n': n_flag = 1; break;
			// case 'e': e_flag = 1; break;
			// case 'f': f_flag = 1; break;
			case 'h': opts->display_help = 1; break;
			// case 'i': i_flag = 1; break;
			// case 't': t_flag = 1; break;
			// case 'x': x_flag = 1; break;
			// case 'y': y_flag = 1; break;
			// case 'T': T_flag = 1; break;

			// case 'b': b_value = optarg; break;
			case 'c': opts->console_name = optarg; break;
			case 'd': opts->dump_filename = optarg; break;
			case 'm': opts->mapper_name = optarg; break;
			case 'p': opts->program_filename = optarg; break;
			case 's': opts->lua_filename = optarg; break;
			// case 'v': v_value = optarg; break;
			// case 'C': C_value = optarg; break;
			// case 'L': L_value = optarg; break;
			// case 'K': K_value = optarg; break;
			// case 'O': O_value = optarg; break;
			// case 'P': P_value = optarg; break;
			// case 'S': S_value = optarg; break;
			// case 'W': W_value = optarg; break;

			case '?':
				if(
				// ( optopt == 'b' ) ||
				   ( optopt == 'c' )
				|| ( optopt == 'd' )
				|| ( optopt == 'm' )
				|| ( optopt == 'p' )
				|| ( optopt == 's' )
				// || ( optopt == 'v' )
				// || ( optopt == 'C' )
				// || ( optopt == 'L' )
				// || ( optopt == 'K' )
				// || ( optopt == 'O' )
				// || ( optopt == 'P' )
				// || ( optopt == 'S' )
				// || ( optopt == 'W' )
				){
					log_err("Option -%c requires an argument.", optopt);
					//goto error;
					return NULL;

				} else if ( isprint(optopt)) {
					log_err("Unknown option -%c .", optopt);
					return NULL;
					// goto error;
				} else {
					log_err("Unknown option character '\\x%x'", optopt);
					// goto error;
					return NULL;
				}
				log_err("Improper arguements passed");
					// goto error;
					return NULL;
				break;
			default:
				//sentinel("getopt failed to catch all arg cases");
				printf("getopt failed to catch all arg cases");
				return 0;
		}
	
	}

	// debug("flags= o:%d n:%d e:%d f:%d h:%d i:%d t:%d x:%d y:%d T:%d", 
	// 	o_flag, n_flag, e_flag, f_flag, h_flag, i_flag, t_flag, x_flag, y_flag, T_flag ); 
	// debug("args= b:%s c:%s d:%s m:%s p:%s", b_value, c_value, d_value, m_value, p_value); 
	// debug("args= s:%s v:%s C:%s L:%s K:%s", s_value, v_value, C_value, L_value, K_value); 
	// debug("args= O:%s P:%s S:%s W:%s", O_value, P_value, S_value, W_value); 

	for( index = optind; index < argc; index++) {
		log_err("Non-option arguement: %s \n", argv[index]);
	}

	//TODO display better help message 
	if (opts->display_help) {
		printf("You've asked for help but the help message still needs created...\n");
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
lua_State *lua_init() {
	lua_State *L = luaL_newstate(); //opens Lua
	luaL_openlibs(L); //opens the standard libraries

	// Register C functions that can be called from Lua.
	// lua_pushcfunction(L, lua_dictionary_call);
	lua_pushcfunction(L, lua_usb_vend_xfr);
	lua_setglobal(L, "usb_vend_xfr");
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

	/* 
	if(rom->fileptr != NULL) {
		fclose(rom->fileptr);
	}
	*/
}

int main(int argc, char *argv[]) 
{	
	// int operation = 0;	//used to denote the overall operation being requested flash/dump/verify etc
	// opterr = 0;

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

	//Start up Lua
	L = lua_init();
	
/*
	//flags about input files only used for writes
	if ( p_value || i_flag || C_value || P_value || S_value || W_value ) {
		check( d_value == NULL, "input args conflict can't program and dump in same operation.");
		operation = WRITE;
	}

	//flags about output files used for reads
	if ( d_value ) {
		check( e_flag == 0, "input args conflict can't erase and dump in same operation.");
		operation = READ;
	}
*/

//TODO all commandline args must be collected and passed into lua

	//create file object/struct 
//	rom_image *rom = malloc( sizeof(rom_image));	
//
//	check_mem(rom);
//	init_rom_elements(rom);


	
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

	//open INL retro prog with firmware version 2.0 or greater
	//if (K_value != NULL) {
	//	//TODO take K_value option to connect to different version kazzo
	//}
	


	
	//provide dictionary.c with pointer to transfer so it can update it's local pointer
	//init_dictionary( transfer );

	// Pass args to Lua
	lua_pushstring(L, opts->console_name);
	lua_setglobal(L, "console_name");

	lua_pushstring(L, opts->mapper_name);
	lua_setglobal(L, "mapper_name");

	lua_pushstring(L, opts->dump_filename);
	lua_setglobal(L, "dump_filename");

	lua_pushstring(L, opts->program_filename);
	lua_setglobal(L, "flash_filename");

	// USB device is open, pass args and control over to Lua.
	// If lua_filename isn't set from args, use default script.
	const char *DEFAULT_SCRIPT = "scripts/inlretro.lua";
	char *script = DEFAULT_SCRIPT;
	if (opts->lua_filename) {
		script = opts->lua_filename;
	}
	check(!(luaL_loadfile(L, script) || lua_pcall(L, 0, 0, 0)),
		   "cannot run config. file: %s", lua_tostring(L, -1));
	
	//program flow doesn't come back to this point until script call ends/returns
	
/*
	//create board object/struct 
	cartridge *cart = malloc( sizeof(cartridge));	
	check_mem(cart);

	//set all cart elements to UNKNOWN and allocate memory object within
	init_cart_elements(cart);

	//TEST flag for development use to provide means to only call test.c functions
	if (T_flag) {
		test_function( cart, transfer );
		goto close;
	}

	// -x flag turns off all autodection
	if (!x_flag) {
		//attempt to detect board inserted in device
		check(!detect_console( cart, transfer ), "Problem detecting cartridge.");	

		//detect mapper as much as possible
		check(!detect_mirroring( cart, transfer ), "Problem detecting cart mirroring.");	

		//TODO first step for SNES is mapping mode
	
		//By this point we know a lot about the cartridge but for things like NES discrete
		//mappers we'll have to play around with the memory to determine exact mapper
		//detect board manufacturer/flash memories as much as possible
		check(!detect_map_mem( cart, transfer, operation ), "Problem detecting cart map & memory.");	

		//detect rom sizes as much as possible

	} else {
		printf("auto-detection turned off\n");
	}

	//read in user files/args that glean info about expected board

	//compare detections to user args and get permission to continue if there are discrepencies

	//just dump based on input args for now
	if ( d_value ) {
		//TODO input arg checking
		if ( c_value ) {
			if ( strcmp( "NES", c_value ) == 0 ) rom->console = NES_CART;
			if ( strcmp( "FC", c_value ) == 0 ) rom->console = FC_CART;
			if ( strcmp( "SNES", c_value ) == 0 ) rom->console = SNES_CART;
		}
		//TODO interpret provided file extension to determine desired console
		debug("console is: %c", rom->console);

		if ( m_value ) rom->mapper = atoi(m_value);		
		debug("mapper is: %d", rom->mapper);

		if ( rom->mapper == NROM ) {
			rom->prg_size = 32 * KByte;
			rom->chr_size = 8 * KByte;
		}
		
		//TODO check if enough input args were provided or can be detected
		check( !create_file( rom, d_value ), "Unable to create file %s", d_value);		

		//collected as much info as can dump cart without reading any data
		check( !dump_cart( transfer, rom, cart ), "Error while dumping cart");
		debug("done dumping, closing");

		check(! close_rom( rom ), "Problem closing file");
		rom->fileptr = NULL;
		debug("closed");
	}
	//if flashing, determine if erasures are necessary and where

	//erase required sectors of flash

	//forced to erase board regardless of current status
	//if (e_flag || p_value) {
	if (e_flag) {
		erase_nes( transfer );
	}

	//if flashing determine auto-doubling for oversized flash

	//determine if rom can be flashed in a manner to make board compatible with rom
	//ie CNROM/colordreams can be over flashed to play NROM
	//BNROM can be overflashed to simulate UNROM
	//SUROM can be overflashed to run as SNROM

	//determine if snes input rom needs deinterleaved

	if ( p_value ) {
		//program file provided at commandline
		check( !open_rom( rom, p_value ), "Problem opening file %s", p_value);
		detect_file( rom );

		check( !flash_cart( transfer, rom, cart ), "Error while flashing cart");
		debug("done flashing, closing");

		check(! close_rom( rom ), "Problem closing file");
		rom->fileptr = NULL;
		debug("closed");
	}
*/


	//dump or program data based on user args

	//find some fun trivia to present to user while waiting for flash operatoin..?

	//perform CRC checking to check integrity of dump/flash operation

	//handle simple LED ON/OFF within main for now
	//TODO cut this newbie code out of here
/*
	int xfr_cnt = 0;
	uint8_t rbuf[8];

	int i;

	if (o_flag | n_flag) {
		printf("before return buffer: ");
		for (i = 0; i < 8; i++) {
			rbuf[i] = 7;
			printf("%x ", rbuf[i]);
		}
		printf("\n");
		transfer->endpoint = USB_IN;
		transfer->request = DICT_PINPORT;
		if (o_flag) transfer->wValueLSB = LED_ON;
		if (n_flag) transfer->wValueLSB = LED_OFF;
		transfer->data = rbuf;
		transfer->wLength = 1;

		//send command
		xfr_cnt = usb_transfer( transfer );

		printf("total bytes xfrd: %d \n", xfr_cnt);
		printf("after buffer: ");
		for (i = 0; i < 8; i++) {
			printf("%x ", rbuf[i]);
		}
		printf("\n");
	}
*/

	cleanup(context, transfer, L);
	return 0;

// 'check' macros goto this label if they fail.
error:
	printf("Fatal error encountered, exiting.\n");
	cleanup(context, transfer, L);
	return 1;

}
