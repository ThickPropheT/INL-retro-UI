#include "inlprog_core.h"

// EXPORT bool TestUsbMem() {
	// libusb_context *context = NULL;
	
	// USBtransfer *transfer = usb_init_inldevice(context, LIBUSB_LOG_LEVEL_NONE);
	// bool no_mem = !transfer;
	
	// usb_close_inldevice(context, transfer);
	// return no_mem;
// }

// Returns true if given number is a power of 2, and at least minimum size.
int isValidROMSize(int x, int min) {
	return ((x & (x - 1)) == 0) && x >= min;
}

// Take a mixed-case string and convert to only lowercase.
void strToLower(char *str) {
	while(*str) {
		*str = tolower(*str);
		str++;
	}
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

void lua_registerFunctions(lua_State *L)
{
	// Register C functions that can be called from Lua.
	lua_pushcfunction(L, lua_usb_vend_xfr);
	lua_setglobal(L, "usb_vend_xfr\0");
}

lua_State *open_lua()
{
	lua_State *L = luaL_newstate(); //opens Lua
	luaL_openlibs(L); //opens the standard libraries
	
	lua_registerFunctions(L);
	
	return L;
}

// Setup Lua environment.
lua_State *lua_init(INLOptions *opts) {

	lua_State *L = open_lua();

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

	lua_pushinteger(L, opts->rom_size_kbyte);
	lua_setglobal(L, "rom_size_kbyte");

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
