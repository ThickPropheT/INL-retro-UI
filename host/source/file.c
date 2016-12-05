#include "file.h"

#define SIZE_NES_HEADER 16
#define SIZE_PRG_BANK 16384
#define SIZE_CHR_BANK 8192

int detect_file( rom_image *rom_info, char *filename ) 
{
	int rv = 0;
//	int index = 0;
	FILE *fileptr = NULL;
//warn	uint8_t data[128];

	//first open file
	fileptr = fopen( filename, "rb");
	//returns file ptr on success, NULL on fail
	check( fileptr, "Unable to open file: %s in read binary mode", filename); 	

	//then determine file type
	uint8_t header[SIZE_NES_HEADER];
	//size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
	rv = fread( header, sizeof(header[0]), (sizeof(header)/sizeof(header[0])), fileptr);
	check( rv = sizeof(header), "Unable to read NES header");

	//for ( index = 0; index < SIZE_NES_HEADER; index++ ) {
	//	debug("header byte #%d = h%x c%c", index, header[index], header[index]);
	//}

	//0-3: Constant $4E $45 $53 $1A ("NES" followed by MS-DOS end-of-file)
	if ( (header[0]=='N') && (header[1]=='E') && (header[2]=='S') && (header[3]==0x1A) ) {
		debug("detected ines header");
		rom_info->console = NES_CART;
	} else {
		debug("only ines files currently accepted as input");
		goto error;
	}

	//4: Size of PRG ROM in 16 KB units
	rom_info->prg_size = header[4] * SIZE_PRG_BANK;
	debug("PRG ROM size= %d", rom_info->prg_size);

	//5: Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
	rom_info->chr_size = header[5] * SIZE_CHR_BANK;
	debug("CHR ROM size= %d", rom_info->chr_size);

	//6: Flags 6
	//   76543210
	//   ||||||||
	//   ||||+||+- 0xx0: vertical arrangement/horizontal mirroring (CIRAM A10 = PPU A11)
	//   |||| ||   0xx1: horizontal arrangement/vertical mirroring (CIRAM A10 = PPU A10)
	//   |||| ||   1xxx: four-screen VRAM
	//   |||| |+-- 1: Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory
	//   |||| +--- 1: 512-byte trainer at $7000-$71FF (stored before PRG data)
	//   ++++----- Lower nybble of mapper number
	rom_info->mapper = (header[6]>>4);
	rom_info->mapper |= (header[7] & 0xF0);
	debug("mapper #%d", rom_info->mapper);
	rom_info->mirroring = header[6] & 0x09;	//0b0000 1001
	debug("mirroring:%x", rom_info->mirroring);
	//7: Flags 7
	//   76543210
	//   ||||||||
	//   |||||||+- VS Unisystem
	//   ||||||+-- PlayChoice-10 (8KB of Hint Screen data stored after CHR data)
	//   ||||++--- If equal to 2, flags 8-15 are in NES 2.0 format
	//   ++++----- Upper nybble of mapper number

	//8: Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility; see PRG RAM circuit)
	//9: Flags 9
	//10: Flags 10 (unofficial)
	//11-15: Zero filled
	
	//close file
	check( fclose(fileptr) == SUCCESS, "Unable to close file");
	fileptr = NULL;
	
	return 0;

error:

	if (fileptr) {
		fclose(fileptr);
	}

	return -1;

}
