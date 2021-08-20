#include "inlprog_core.h"

const char *HELP =  "Usage: inlretro [options]\n\n"\
					"Options/Flags:\n"\
					"  --console=console, -c console\t\t\tConsole port, {GBA,GENESIS,N64,NES}\n"\
					"  --dump_filename=filename, -d filename\t\tIf provided, dump cartridge ROMs to this filename\n"\
					"  --dump_ram_filename=filename, -a filename\tIf provided write ram to this filename\n"\
					"  --help, -h\t\t\t\t\tDisplays this message.\n"\
					"  --lua_filename=filename, -s filename\t\tIf provided, use this script for main application logic\n"\
					"  --mapper=mapper, -m mapper\t\t\tNES, SNES, GB consoles only, mapper ASIC on cartridge\n"\
					"  \t\t\t\t\t\tNES:\t{action53,bnrom,cdream,cninja,cnrom,dualport,easynsf,fme7,\n"\
					"  \t\t\t\t\t\t\t mapper30,mmc1,mmc3,mmc4,mmc5,nrom,unrom}\n"\
					"  \t\t\t\t\t\tGB:\t{mbc1,romonly}\n"\
					"  \t\t\t\t\t\tSNES:\t{lorom,hirom}\n"\
					"  --nes_prg_rom_size_kbyte=size, -x size_kbytes\tNES-only, size of PRG-ROM in kilobytes\n"\
					"  --nes_chr_rom_size_kbyte=size, -y size_kbytes\tNES-only, size of CHR-ROM in kilobytes\n"\
					"  --rom_size_kbyte=size, -k size_kbytes\t\tSize of ROM in kilobytes, non-NES systems.\n"\
					"  --rom_size_mbit=size, -z size_mbits\t\tSize of ROM in megabits, non-NES systems.\n"\
					"  --verify_filename=filename, -v filename\tIf provided, writeback written rom to this filename\n"\
					"  --wram_size_kbyte=size, -w size_kbytes\tNES-only, size of WRAM in kilobytes\n"\
					"  --write_filename=filename, -p filename\tIf provided, write this data to cartridge\n"\
					"  --write_ram_filename=filename, -b filename\tIf provided write this file's contents to ram\n";

// Parse options and flags, create struct to drive program.
INLOptions* parseOptions(int argc, char *argv[]) {

	// Declare command line flags/options.
	struct option longopts[] = {
		{"help", optional_argument, NULL, 'h'},
		{"console", required_argument, NULL, 'c'},
		{"mapper", optional_argument, NULL, 'm'},
		{"dump_filename", optional_argument, NULL, 'd'},
		{"dump_ram_filename", optional_argument, NULL, 'a'},
		{"write_filename", optional_argument, NULL, 'p'},
		{"write_ram_filename", optional_argument, NULL, 'b'},
		{"verify_filename", optional_argument, NULL, 'v'},
		{"lua_filename", optional_argument, NULL, 's'},
		{"nes_prg_rom_size_kbyte", optional_argument, NULL, 'x'},
		{"nes_chr_rom_size_kbyte", optional_argument, NULL, 'y'},
		{"wram_size_kbyte", optional_argument, NULL, 'w'},
		{"rom_size_kbyte", optional_argument, NULL, 'k'},
		{"rom_size_mbit", optional_argument, NULL, 'z'},
		{0, 0, 0, 0} // longopts must end in {0, 0, 0, 0}
	};

	// FLAG_FORMAT must be kept in sync with any short options used in longopts.
	const char *FLAG_FORMAT = "a:b:hc:d:k:m:p:s:v:w:x:y:z:";
	int index = 0;
	int rv = 0;
	int kbyte = 0;
	opterr = 0;

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
	while((rv = getopt_long(argc, argv, FLAG_FORMAT, longopts, NULL)) != -1) {
		
		switch(rv) {

			case 'a': opts->ramdump_filename = optarg; break;
			case 'b': opts->ramwrite_filename = optarg; break;
			case 'h': opts->display_help = 1; break;
			case 'c': opts->console_name = optarg; break;
			case 'd': opts->dump_filename = optarg; break;
			case 'k': opts->rom_size_kbyte = atoi(optarg); break;
			case 'm': opts->mapper_name = optarg; break;
			case 'p': opts->program_filename = optarg; break;
			case 's': opts->lua_filename = optarg; break;
			case 'v': opts->verify_filename = optarg; break;
			case 'w': opts->wram_size_kb = atoi(optarg); break;
			case 'x': opts->prg_rom_size_kb = atoi(optarg); break;
			case 'y': opts->chr_rom_size_kb = atoi(optarg); break;
			case 'z':
				kbyte = atoi(optarg) * 128;
				if (opts->rom_size_kbyte && opts->rom_size_kbyte != kbyte) {
					printf("rom_size_mbit disagrees with rom_size_kbyte! Using %d Kb as rom size.\n",  kbyte);
				}
				opts->rom_size_kbyte = kbyte;
				break;
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

	// Handle console, mapper as case-insensitive configuration.
	strToLower(opts->console_name);
	strToLower(opts->mapper_name);
	return opts;
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
	if (strcmp("nes", opts->console_name) == 0) {
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

	if ((strcmp("gba", opts->console_name) == 0) ||
		(strcmp("n64", opts->console_name) == 0)) {
		if (opts->rom_size_kbyte <= 0) {
			printf("ROM size must be greater than 0 kilobytes.\n");
			return 1;
		}
		if (opts->rom_size_kbyte % 128) {
			printf("ROM size for this system must translate into megabits with no kilobyte remainder.\n");
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

	lua_getglobal(L, "main\0");

	check(!lua_pcall(L, 0, 0, 0),
		"Cannot run main. file: %s", lua_tostring(L, -1));
	
	// Program flow doesn't come back to this point until script call ends/returns.
	cleanup(context, transfer, L);
	return 0;

// 'check' macros goto this label if they fail.
error:
	printf("Fatal error encountered, exiting.\n");
	cleanup(context, transfer, L);
	return 1;
}
