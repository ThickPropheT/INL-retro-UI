#ifndef _enums_h
#define _enums_h

//One concise place to list all enums used for setting cartridge and memory elements

//used to denote when any cartridge element is not known
#define UNKNOWN 	-1	

//console options
#define NES_CART 	'N'
#define FC_CART 	'F'
#define SNES_CART 	'S'
#define BKWD_CART 	'B'

enum mirroring {
	MIR_FIXED = 10,
	MIR_ANROM,
	MIR_MMC1,
	MIR_MMC3
};


#endif
