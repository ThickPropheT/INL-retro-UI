#include "sega.h"

//=================================================================================================
//
//	SEGA operations
//	This file includes all the sega functions possible to be called from the sega dictionary.
//
//	See description of the commands contained here in shared/shared_dictionaries.h
//
//=================================================================================================

/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated function.
 * 	shared_dict_sega.h is used in both host and fw to ensure opcodes/names align
 * Pre: Macros must be defined in firmware pinport.h
 * 	opcode must be defined in shared_dict_sega.h
 * Post:function call complete.
 * Rtn: SUCCESS if opcode found and completed, error if opcode not present or other problem.
 */
uint8_t sega_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata )
{

#define	RD_LEN	0
#define	RD0	1
#define	RD1	2

#define	BYTE_LEN 1
#define	HWORD_LEN 2
	
	switch (opcode) { 
//		//no return value:
		case SEGA_WR:	
			sega_wr( operand, miscdata );
			break;

		//8bit return values:
		case SEGA_RD:
			rdata[RD_LEN] = BYTE_LEN;
			rdata[RD0] = sega_rd( operand );
			break;
		default:
			 //macro doesn't exist
			 return ERR_UNKN_SEGA_OPCODE;
	}
	
	return SUCCESS;

}

uint8_t	sega_rd( uint16_t addr )
{
	return 0xAA;
}


void	sega_wr( uint16_t addr, uint8_t data )
{
	return;
}
