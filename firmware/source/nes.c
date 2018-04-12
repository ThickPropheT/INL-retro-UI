#include "nes.h"

//=================================================================================================
//
//	NES operations
//	This file includes all the nes functions possible to be called from the nes dictionary.
//
//	See description of the commands contained here in shared/shared_dictionaries.h
//
//=================================================================================================

/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated function.
 * 	shared_dict_nes.h is used in both host and fw to ensure opcodes/names align
 * Pre: Macros must be defined in firmware pinport.h
 * 	opcode must be defined in shared_dict_nes.h
 * Post:function call complete.
 * Rtn: SUCCESS if opcode found and completed, error if opcode not present or other problem.
 */
uint8_t nes_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata )
{

#define	RD_LEN	0
#define	RD0	1
#define	RD1	2

#define	BYTE_LEN 1
#define	HWORD_LEN 2
	
	switch (opcode) { 
//		//no return value:
		case DISCRETE_EXP0_PRGROM_WR:	
			discrete_exp0_prgrom_wr( operand, miscdata );
			break;
		case NES_PPU_WR:	
			nes_ppu_wr( operand, miscdata );
			break;
		case NES_CPU_WR:	
			nes_cpu_wr( operand, miscdata );
			break;
//		case DISCRETE_EXP0_MAPPER_WR:	
//			discrete_exp0_mapper_wr( operand, miscdata );
//			break;

		//8bit return values:
//		case EMULATE_NES_CPU_RD:
//			*data = emulate_nes_cpu_rd( addrH, addrL );
//			break;
		case NES_CPU_RD:
			rdata[RD_LEN] = BYTE_LEN;
			rdata[RD0] = nes_cpu_rd( operand );
			break;
		case NES_PPU_RD:
			rdata[RD_LEN] = BYTE_LEN;
			rdata[RD0] = nes_ppu_rd( operand );
			break;
		case CIRAM_A10_MIRROR:
			rdata[RD_LEN] = BYTE_LEN;
			rdata[RD0] = ciram_a10_mirroring( );
			break;
		default:
			 //macro doesn't exist
			 return ERR_UNKN_NES_OPCODE;
	}
	
	return SUCCESS;

}


/* Desc: Discrete board PRG-ROM only write, does not write to mapper
 * 	PRG-ROM /WE <- EXP0 w/PU
 * 	PRG-ROM /OE <- /ROMSEL
 * 	PRG-ROM /CE <- GND
 * 	PRG-ROM write: /WE & /CE low, /OE high
 * 	mapper '161 CLK  <- /ROMSEL
 * 	mapper '161 /LOAD <- PRG R/W
 * 	mapper '161 /LOAD must be low on rising edge of CLK to latch data
 * 	This is a /WE controlled write. Address latched on falling edge, 
 *	and data latched on rising edge EXP0
 * Note:addrH bit7 has no effect (ends up on PPU /A13)
 * 	/ROMSEL, M2, & PRG R/W signals untouched
 * Pre: nes_init() setup of io pins
 * Post:data latched by PRG-ROM, mapper register unaffected
 * 	address left on bus
 * 	data left on bus, but pullup only
 * 	EXP0 left pulled up
 * Rtn:	None
 */
void	discrete_exp0_prgrom_wr( uint16_t addr, uint8_t data )
{
	ADDR_SET(addr);

	DATA_OP();
	DATA_SET(data);

	EXP0_OP();	//Tas = 0ns, Tah = 30ns
	EXP0_LO();
	EXP0_IP_PU();	//Twp = 40ns, Tds = 40ns, Tdh = 0ns
	//16Mhz avr clk = 62.5ns period guarantees timing reqts
	DATA_IP();
}


/* Desc: Discrete board MAPPER write without bus conflicts
 * 	will also write to PRG-ROM, but PRG-ROM shouldn't output
 * 	data while writting to mapper.  Thus removing need for bank table.
 * 	NOTE: I think it would be possible to write one value to mapper
 * 	and another value to PRG-ROM.
 * 	PRG-ROM /WE <- EXP0 w/PU
 * 	PRG-ROM /OE <- /ROMSEL
 * 	PRG-ROM /CE <- GND
 * 	PRG-ROM write: /WE & /CE low, /OE high
 * 	mapper '161 CLK  <- /ROMSEL
 * 	mapper '161 /LOAD <- PRG R/W
 * 	mapper '161 /LOAD must be low on rising edge of CLK to latch data
 * Note:addrH bit7 has no effect (ends up on PPU /A13)
 * 	M2 signal untouched
 * Pre: nes_init() setup of io pins
 * Post:data latched by MAPPER, will also be written to PRG-ROM afterwards
 * 	address left on bus
 * 	data left on bus, but pullup only
 * 	EXP0 left pulled up
 * Rtn:	None
 */
//void	discrete_exp0_mapper_wr( uint16_t addr, uint8_t data )
//{
//	//Float EXP0 as it should be in NES
//	EXP0_IP_FL();
//	//EXP0_OP();	//tas = 0ns, tah = 30ns
//	//EXP0_LO();
//
//	//need for whole function
//	//_DATA_OP();
//
//	//set addrL
//	//ADDR_OUT = addrL;
//	//latch addrH
//	//DATA_OUT = addrH;
//	//_AHL_CLK();	
//	ADDR_SET(addr);
//
//	//PRG R/W LO
//	PRGRW_LO();
//
//	//put data on bus
//	DATA_OP();
//	DATA_SET(data);
//
//	//set M2 and /ROMSEL
//	MCO_HI();
//	if( addr >= 0x8000 ) {	//addressing cart rom space
//		ROMSEL_LO();	//romsel trails M2 during CPU operations
//	}
//
//	//give some time
//	NOP();
//	NOP();
//
//	//latch data to cart memory/mapper
//	MCO_LO();
//	ROMSEL_HI();
//
//	//retore PRG R/W to default
//	PRGRW_HI();
//
//	EXP0_IP_PU();	//Twp = 40ns, Tds = 40ns, Tdh = 0ns
//	//Free data bus
//	DATA_IP();
//
//	return;
//
//	/*
//	ADDR_SET(addr);
//
//	DATA_OP();
//	DATA_SET(data);
//
//	//start write to PRG-ROM (latch address)
//	exp0_op();	//tas = 0ns, tah = 30ns
//	exp0_lo();
//
//	//enable write to mapper PRG R/W LO
//	PRGRW_LO();
//	ROMSEL_LO();	//fact that it's low for such a short time might also if PRG-ROM does output data
//
//	NOP();		//AVR didn't need this delay
//	NOP();		//AVR didn't need this delay
//	NOP();		//AVR didn't need this delay
//	NOP();		//AVR didn't need this delay
//	NOP();		//AVR didn't need this delay
//	NOP();		//AVR didn't need this delay
//	//clock mapper register, should not enable PRG-ROM output since /WE low
//	NOP();		//AVR didn't need this delay
//	NOP();		//AVR didn't need this delay
//	ROMSEL_HI();	//data latched on rising edge
//
//	//Could output other data here that would like to be written to PRG-ROM
//	//I'm not certain an actual write gets applied to PRG-ROM as /OE is supposed to be high whole time..
//
//	NOP();		//AVR didn't need this delay
//	//return to default
//	PRGRW_HI();
//
//	EXP0_IP_PU();	//Twp = 40ns, Tds = 40ns, Tdh = 0ns
//	//16Mhz avr clk = 62.5ns period guarantees timing reqts
//	DATA_IP();
//	*/
//}


//	
//	/* Desc:Emulate NES CPU Read as best possible
//	 * 	decode A15 from addrH to set /ROMSEL as expected
//	 * 	float EXP0
//	 * 	toggle M2 as NES would
//	 * 	insert some NOP's in to be slow like NES
//	 * Note:not the fastest read operation
//	 * Pre: nes_init() setup of io pins
//	 * Post:address left on bus
//	 * 	data bus left clear
//	 * 	EXP0 left floating
//	 * Rtn:	Byte read from PRG-ROM at addrHL
//	 */
//	uint8_t	emulate_nes_cpu_rd( uint8_t addrH, uint8_t addrL )
//	{
//		uint8_t	read;	//return value
//	
//		//m2 should be low as it aids in disabling WRAM
//		//this is also m2 state at beginging of CPU cycle
//		//all these pins should already be in this state, but
//		//go ahead and setup just to be sure since we're trying
//		//to be as accurate as possible
//		_EXP0_FLT();	//this could have been left pulled up
//		_M2_LO();	//start of CPU cycle
//		_ROMSEL_HI();	//trails M2
//		_PRGRW_RD();	//happens just after M2
//	
//		//set address bus
//		ADDR_OUT = addrL;
//		_ADDRH_SET(addrH);
//		
//		//couple NOP's to wait a bit
//		NOP();
//		NOP();
//	
//		//set M2 and /ROMSEL
//		if( addrH >= 0x80 ) {	//addressing cart rom space
//			_M2_HI();	
//			_ROMSEL_LO();	//romsel trails M2 during CPU operations
//		} else {
//			_M2_HI();
//		}
//	
//		//couple more NOP's waiting for data
//		NOP();
//		NOP();
//		NOP();
//		NOP();
//		NOP();
//		NOP();
//	
//		//latch data
//		read = DATA_IN;
//	
//		//return bus to default
//		_M2_LO();
//		_ROMSEL_HI();
//		
//		return read;
//	}
//	
/* Desc:NES CPU Read without being so slow
 * 	decode A15 from addrH to set /ROMSEL as expected
 * 	float EXP0
 * 	toggle M2 as NES would
 * Pre: nes_init() setup of io pins
 * Post:address left on bus
 * 	data bus left clear
 * 	EXP0 left floating
 * Rtn:	Byte read from PRG-ROM at addrHL
 */
uint8_t	nes_cpu_rd( uint16_t addr )
{
	uint8_t	read;	//return value

	//set address bus
	ADDR_SET(addr);
	
	//set M2 and /ROMSEL
	MCO_HI();
	if( addr >= 0x8000 ) {	//addressing cart rom space
		ROMSEL_LO();	//romsel trails M2 during CPU operations
	}

	//couple more NOP's waiting for data
	//zero nop's returned previous databus value
	NOP();	//one nop got most of the bits right
	NOP();	//two nop got all the bits right
	NOP();	//add third nop for some extra
	NOP();	//one more can't hurt
	//might need to wait longer for some carts...

	//latch data
	DATA_RD(read);

	//return bus to default
	MCO_LO();
	ROMSEL_HI();
	
	return read;
}

/* Desc:NES CPU Write
 *	Just as you would expect NES's CPU to perform
 *	A15 decoded to enable /ROMSEL
 *	This ends up as a M2 and/or /ROMSEL controlled write
 * Note:addrH bit7 has no effect (ends up on PPU /A13)
 *	EXP0 floating
 * Pre: nes_init() setup of io pins
 * Post:data latched by anything listening on the bus
 * 	address left on bus
 * 	data left on bus, but pullup only
 * Rtn:	None
 */
void	nes_cpu_wr( uint16_t addr, uint8_t data )
{
	//Float EXP0 as it should be in NES
	EXP0_IP_FL();

	//need for whole function
	//_DATA_OP();

	//set addrL
	//ADDR_OUT = addrL;
	//latch addrH
	//DATA_OUT = addrH;
	//_AHL_CLK();	
	ADDR_SET(addr);

	//PRG R/W LO
	PRGRW_LO();

	//put data on bus
	DATA_OP();
	DATA_SET(data);

	//set M2 and /ROMSEL
	MCO_HI();
	if( addr >= 0x8000 ) {	//addressing cart rom space
		ROMSEL_LO();	//romsel trails M2 during CPU operations
	}

	//give some time
	NOP();
	NOP();

	//latch data to cart memory/mapper
	MCO_LO();
	ROMSEL_HI();

	//retore PRG R/W to default
	PRGRW_HI();

	//Free data bus
	DATA_IP();
}

/* Desc:NES PPU Read 
 * 	decode A13 from addrH to set /A13 as expected
 * Pre: nes_init() setup of io pins
 * Post:address left on bus
 * 	data bus left clear
 * Rtn:	Byte read from CHR-ROM/RAM at addrHL
 */
uint8_t	nes_ppu_rd( uint16_t addr )
{
	uint8_t	read;	//return value

	//addr with PPU /A13
	if (addr < 0x2000) { //below $2000 A13 clear, /A13 set
		addr |= PPU_A13N_WORD;
	} //above PPU $1FFF, A13 set, /A13 clear 

	ADDR_SET( addr );
	
	//set CHR /RD and /WR
	CSRD_LO();

	//couple more NOP's waiting for data
	//zero nop's returned previous databus value
	NOP();	//one nop got most of the bits right
	NOP();	//two nop got all the bits right
	NOP();	//add third nop for some extra
	NOP();	//one more can't hurt
	//might need to wait longer for some carts...

	//latch data
	DATA_RD(read);

	//return bus to default
	CSRD_HI();
	
	return read;
}

/* Desc:NES PPU Write 
 * 	decode A13 from addrH to set /A13 as expected
 *	flash: address clocked falling edge, data rising edge of /WE
 * Pre: nes_init() setup of io pins
 * Post:data written to addrHL
 *	address left on bus
 * 	data bus left clear
 * Rtn:	None
 */

void	nes_ppu_wr( uint16_t addr, uint8_t data )
{

	//addr with PPU /A13
	if (addr < 0x2000) { //below $2000 A13 clear, /A13 set
		addr |= PPU_A13N_WORD;
	} //above PPU $1FFF, A13 set, /A13 clear 

	ADDR_SET( addr );

	//put data on bus
	DATA_OP();
	DATA_SET(data);

	NOP();
	
	//set CHR /RD and /WR
	CSWR_LO();

	//might need to wait longer for some carts...
	NOP();	//one can't hurt

	//latch data to memory
	CSWR_HI();

	//clear data bus
	DATA_IP();
	
}


/* Desc:PPU CIRAM A10 NT arrangement sense
 *	Toggle A11 and A10 and read back CIRAM A10
 *	report back if vert/horiz/1scnA/1scnB
 *	reports nesdev defined mirroring
 *	does not report Nintendo's "Name Table Arrangement"
 * Pre: nes_init() setup of io pins
 * Post:address left on bus
 * Rtn:	MIR_VERT, MIR_HORIZ, MIR_1SCNA, MIR_1SCNB
 *	errors not really possible since all combinations
 *	of CIRAM A10 level designate something valid
 */
uint8_t	ciram_a10_mirroring( void )
{
	uint16_t readV, readH;

	//set A10, clear A11
	ADDRH(A10_BYTE);
	CIA10_RD(readV);

	//set A11, clear A10
	ADDRH(A11_BYTE);
	CIA10_RD(readH);

	//if CIRAM A10 was always low -> 1 screen A
	if ((readV==0) & (readH==0))	return MIR_1SCNA;
	//if CIRAM A10 was always hight -> 1screen B
	if ((readV!=0) & (readH!=0))	return MIR_1SCNB;
	//if CIRAM A10 toggled with A10 -> Vertical mirroring, horizontal arrangement
	if ((readV!=0) & (readH==0))	return MIR_VERT;
	//if CIRAM A10 toggled with A11 -> Horizontal mirroring, vertical arrangement
	if ((readV==0) & (readH!=0))	return MIR_HORZ;

	//shouldn't be here...
	return GEN_FAIL;
}

/* Desc:NES CPU Page Read with optional USB polling
 * 	decode A15 from addrH to set /ROMSEL as expected
 * 	float EXP0
 * 	toggle M2 as NES would
 *	if poll is true calls usbdrv.h usbPoll fuction
 *	this is needed to keep from timing out when double buffering usb data
 * Pre: nes_init() setup of io pins
 *	num_bytes can't exceed 256B page boundary
 * Post:address left on bus
 * 	data bus left clear
 * 	EXP0 left floating
 *	data buffer filled starting at first to last
 * Rtn:	Index of last byte read
 */
uint8_t nes_cpu_page_rd_poll( uint8_t *data, uint8_t addrH, uint8_t first, uint8_t len, uint8_t poll )
{
	uint8_t i;

	//set address bus
	ADDRH(addrH);
	
	//set M2 and /ROMSEL
	MCO_HI();
	if( addrH >= 0x80 ) {	//addressing cart rom space
		ROMSEL_LO();	//romsel trails M2 during CPU operations
	}

	//set lower address bits
	ADDRL(first);		//doing this prior to entry and right after latching
	//extra NOP was needed on stm6 as address hadn't settled in time for the very first read
	NOP();	
				//gives longest delay between address out and latching data
	for( i=0; i<=len; i++ ) {
		//testing shows that having this if statement doesn't affect overall dumping speed
		if ( poll == FALSE ) {
			NOP();	//couple more NOP's waiting for data
			NOP();	//one prob good enough considering the if/else
		} else {
			usbPoll();	//Call usbdrv.h usb polling while waiting for data
		}
		//latch data
		DATA_RD(data[i]);
		//set lower address bits
		//ADDRL(++first);	THIS broke things, on stm adapter because macro expands it twice!
		first++;
		ADDRL(first);
	}

	//return bus to default
	MCO_LO();
	ROMSEL_HI();
	
	//return index of last byte read
	return i;
}

/* Desc:NES PPU Page Read with optional USB polling
 * 	decode A13 from addrH to set /A13 as expected
 *	if poll is true calls usbdrv.h usbPoll fuction
 *	this is needed to keep from timing out when double buffering usb data
 * Pre: nes_init() setup of io pins
 *	num_bytes can't exceed 256B page boundary
 * Post:address left on bus
 * 	data bus left clear
 *	data buffer filled starting at first for len number of bytes
 * Rtn:	Index of last byte read
 */
uint8_t nes_ppu_page_rd_poll( uint8_t *data, uint8_t addrH, uint8_t first, uint8_t len, uint8_t poll )
{
	uint8_t i;

	if (addrH < 0x20) { //below $2000 A13 clear, /A13 set
		//ADDRH(addrH | PPU_A13N_BYTE); 
		//Don't do weird stuff like above!  logic inside macro expansions can have weird effects!!
		addrH |= PPU_A13N_BYTE;
		ADDRH(addrH);
	} else { //above PPU $1FFF, A13 set, /A13 clear 
		ADDRH(addrH);
	}

	//set CHR /RD and /WR
	CSRD_LO();

	//set lower address bits
	ADDRL(first);		//doing this prior to entry and right after latching
	NOP();	//adding extra NOP as it was needed on PRG
				//gives longest delay between address out and latching data

	for( i=0; i<=len; i++ ) {
		//couple more NOP's waiting for data
		if ( poll == FALSE ) {
			NOP();	//one prob good enough considering the if/else
			NOP();
		} else {
			usbPoll();
		}
		//latch data
		DATA_RD(data[i]);
		//set lower address bits
		first ++;
		ADDRL(first);
	}

	//return bus to default
	CSRD_HI();
	
	//return index of last byte read
	return i;
}
