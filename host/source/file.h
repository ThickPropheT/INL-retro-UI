#ifndef _file_h
#define _file_h

//uncomment to DEBUG this file alone
#define DEBUG
//"make debug" to get DEBUG msgs on entire program
#include "dbg.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <libusb.h>

//TODO put defintions in separate project wide .h file
#include "cartridge.h"

//cartridge object/struct
typedef struct rom_image{
	int	console;	//console the cart plays in
	int	mapper;		//mapper number of the board
	int	submap;
	int	mapvariant;	
	int	prg_size;
	int	chr_size;
	int	ram_size;
	int	battery;
	int	mirroring;
} rom_image;

int detect_file( rom_image *rom_info, char *filename );

#endif
