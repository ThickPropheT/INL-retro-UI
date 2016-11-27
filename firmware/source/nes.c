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
 * Rtn: SUCCESS if opcode found, ERR_UNKN_NES_OPCODE_24BOP if opcode not present.
 */
uint8_t nes_opcode_24b_operand( uint8_t opcode, uint8_t addrH, uint8_t addrL, uint8_t data )
{
	switch (opcode) { 
		case DISCRETE_EXP0_PRGROM_WR:	
			discrete_exp0_prgrom_wr( addrH, addrL, data );
			break;
		default:
			 //macro doesn't exist
			 return ERR_UNKN_NES_OPCODE_24BOP;
	}
	
	return SUCCESS;

}


/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated function.
 * 	shared_dict_nes.h is used in both host and fw to ensure opcodes/names align
 * Pre: Macros must be defined in firmware pinport.h
 * 	opcode must be defined in shared_dict_nes.h
 * Post:pointer to data updated with return value.
 * Rtn: SUCCESS if opcode found, ERR_UNKN_NES_OPCODE_16BOP_8BRV if opcode not present.
 */
uint8_t nes_opcode_16b_operand_8b_return( uint8_t opcode, uint8_t addrH, uint8_t addrL, uint8_t *data )
{
	switch (opcode) { 
		case EMULATE_NES_CPU_RD:
			*data = emulate_nes_cpu_rd( addrH, addrL );
			break;
		default:
			 //macro doesn't exist
			 return ERR_UNKN_NES_OPCODE_16BOP_8BRV;
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
 * 	This is a /WE controlled write with data latched on rising edge EXP0
 * Note:addrH bit7 has no effect (ends up on PPU /A13)
 * 	/ROMSEL, M2, & PRG R/W signals untouched
 * Pre: nes_init() setup of io pins
 * Post:data latched by PRG-ROM, mapper register unaffected
 * 	address left on bus
 * 	data left on bus, but pullup only
 * 	EXP0 left pulled up
 * Rtn:	None
 */
void	discrete_exp0_prgrom_wr( uint8_t addrH, uint8_t addrL, uint8_t data )
{
	_DATA_OP();
	DATA_OUT = addrH;
	_AHL_CLK();	//addrH latched
	DATA_OUT = data;
	ADDR_OUT = addrL;
	_EXP0_LO();	//Tas = 0ns, Tah = 30ns
	_EXP0_PU();	//Twp = 40ns, Tds = 40ns, Tdh = 0ns
	//16Mhz avr clk = 62.5ns period guarantees timing reqts
	//Need to check with scope to make sure EXP0 P/U effective
	_DATA_IP();
}


/* Desc:Emulate NES CPU Read as best possible
 * 	decode A15 from addrH to set /ROMSEL as expected
 * 	float EXP0
 * 	toggle M2 as NES would
 * 	insert some NOP's in to be slow like NES
 * Note:not the fastest read operation
 * Pre: nes_init() setup of io pins
 * Post:address left on bus
 * 	data bus left clear
 * 	EXP0 left floating
 * Rtn:	None
 */
uint8_t	emulate_nes_cpu_rd( uint8_t addrH, uint8_t addrL )
{
	uint8_t	read;	//return value

	//m2 should be low as it aids in disabling WRAM
	//this is also m2 state at beginging of CPU cycle
	//all these pins should already be in this state, but
	//go ahead and setup just to be sure since we're trying
	//to be as accurate as possible
	_EXP0_FLT();	//this could have been left pulled up
	_M2_LO();	//start of CPU cycle
	_ROMSEL_HI();	//trails M2
	_PRGRW_RD();	//happens just after M2

	//set address bus
	ADDR_OUT = addrL;
	_ADDRH_SET(addrH);
	
	//couple NOP's to wait a bit
	NOP();
	NOP();

	//set M2 and /ROMSEL
	if( addrH >= 0x80 ) {	//addressing cart rom space
		_M2_HI();	
		_ROMSEL_LO();	//romsel trails M2 during CPU operations
	} else {
		_M2_HI();
	}

	//couple more NOP's waiting for data
	NOP();
	NOP();
	NOP();
	NOP();

	//latch data
	read = DATA_IN;

	//return bus to default
	_M2_LO();
	_ROMSEL_HI();
	
	return read;
}
