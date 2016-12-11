#ifndef _shared_enums_h
#define _shared_enums_h

//One concise place to list all enums used 
//for setting cartridge and memory elements on the host
//which end up getting communicated between device and host

//used to denote when any cartridge element is not known
#define UNKNOWN 	0xFF	

//console options
#define NES_CART 	'N'
#define FC_CART 	'F'
#define SNES_CART 	'S'
#define BKWD_CART 	'B'

//NES mappers
#define NROM	0
#define MMC1	1
#define CNROM	2
#define UxROM	3
#define MMC3	4
#define MMC5	5
#define AxROM	7
#define MMC2	9
#define MMC4	10
#define CDREAMS	11
#define A53	28
#define UNROM512 30
#define EZNSF	31
#define BxROM	34
#define RAMBO	64
#define H3001	65	//IREM mapper
#define GxROM	66
#define SUN3	67
#define SUN4	68
#define FME7	69	//SUNSOFT-5 with synth
#define HDIVER	78
#define DxROM	205
//	UNKNOWN 255	don't assign to something meaningful

enum mirroring {
	MIR_1SCNA = 0x10,	//SCNA
	MIR_1SCNB = 0x11,	//SCNB
	MIR_VERT  = 0x12,	//VERT
	MIR_HORIZ = 0x13,	//HORIZ
	MIR_ANROM,
	MIR_MMC1,
	MIR_MMC3,
	MIR_FME7
};

enum operations {
	READ = 10,
	WRITE,	
	CHECK
};

enum buff_mem_type {
	PRGROM = 10,
	CHRROM,
	PRGRAM,
	SNESROM,
	SNESRAM
};

//buffer status values
#define EMPTY 		0x00
#define PROBLEM		0x10
#define USB_UNLOADING	0x80
#define USB_LOADING	0x90
#define USB_FULL	0x98
#define CHECKING	0xC0
#define DUMPING		0xD0
#define STARTDUMP	0xD2
#define DUMPED		0xD8
#define ERASING		0xE0
#define FLASHING	0xF0
#define STARTFLASH	0xF2
#define FLASHED		0xF4
#define FLASH_WAIT	0xF8
#define UNALLOC 	0xFF

#endif
