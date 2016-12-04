#ifndef _cartridge_h
#define _cartridge_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <libusb.h>

//include prior to other file includes
//that way DEBUG can be turned on/off for this file alone
//uncomment to DEBUG this file alone
#define DEBUG
//"make debug" to get DEBUG msgs on entire program
#include "dbg.h"

#include "usb_operations.h"
#include "shared_errors.h"
#include "shared_dictionaries.h"
#include "dictionary.h"
#include "types.h"

#include "io.h"
#include "nes.h"
#include "snes.h"

//cartridge object/struct
typedef struct cartridge{
	int	console;	//console the cart plays in
	int	mapper;		//mapper number of the board
	int	submap;
	int	mapvariant;	
	int	manf;
	int	product;
	int	mirroring;
	int	sound;
	memory	*pri_rom;	//main executable rom (PRG-ROM for NES)
	memory	*sec_rom;	//secondary rom if used (CHR-ROM for NES)
	memory	*save_mem;	//save data memory
	memory	*aux_mem;	//additional memory
	memory	*logic;		//programmable logic
} cartridge;

//console options
#define UNKNOWN 	0	
#define NES_CART 	'N'
#define FC_CART 	'F'
#define SNES_CART 	'S'
#define BKWD_CART 	'B'

int detect_console( cartridge *cart, USBtransfer *transfer );


#endif
