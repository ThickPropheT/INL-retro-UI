#pragma once

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
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

#include "lua_extensions.h"

// Struct used to control functionality.
typedef struct {
	char *console_name;
	char *mapper_name;
	int display_help;

	// NES Functionality
	int chr_rom_size_kb;
	int prg_rom_size_kb;
	int wram_size_kb;

	// General Functionality
	int rom_size_kbyte;
	char *dump_filename;
	char *program_filename;
	char *ramdump_filename;
	char *ramwrite_filename;
	char *verify_filename;

	char *lua_filename;
} INLOptions;

// Returns true if given number is a power of 2, and at least minimum size.
int isValidROMSize(int x, int min);

// Take a mixed-case string and convert to only lowercase.
void strToLower(char *str);

void error_lua (lua_State *L, const char *fmt, ...);

int getglobint (lua_State *L, const char *var);

void load (lua_State *L, const char *fname, int *w, int *h);

lua_State *open_lua();

// Setup Lua environment.
lua_State *lua_init(INLOptions *opts);

// Setup INL USB Device.
USBtransfer *usb_init_inldevice(libusb_context *context, int libusb_log);

// Close and cleanup INL USB Device.
void usb_close_inldevice(libusb_context* context, USBtransfer* transfer);

// Safely cleanup for exiting program and release resources.
void cleanup(libusb_context *context, USBtransfer *transfer, lua_State *L);