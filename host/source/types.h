#ifndef _types_h
#define _types_h

//memory object/struct
typedef struct memory{
	int	manf;
	int	partnum;
	int	volatility;	//sram no batt vs batt, mask rom, erasability, etc
	int	size;		//size of the actual memory excluding grounded address pins etc
	int	addressable;	//addressable size of the memory including grounded address pins etc
	int	width;		//width of data bus as configured
	int	protocol;	//parallel, SPI, I2C, JTAG, custom etc.
	int	sector_size;	//minimum eraseable size in bytes
} memory;




#endif
