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

enum mirroring {
	MIR_FIXED = 10,
	MIR_VERT,
	MIR_HORIZ,
	MIR_ANROM,
	MIR_MMC1,
	MIR_MMC3
};

enum operations {
	READ = 10,
	WRITE,	
	CHECK
};


#endif
