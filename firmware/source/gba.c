#include "gba.h"

//only need this file if connector is present on the device
#ifdef GB_CONN 

//=================================================================================================
//
//	GBA operations
//	This file includes all the gba functions possible to be called from the gba dictionary.
//
//	See description of the commands contained here in shared/shared_dictionaries.h
//
//=================================================================================================

/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated function.
 * 	shared_dict_gba.h is used in both host and fw to ensure opcodes/names align
 * Pre: Macros must be defined in firmware pinport.h
 * 	opcode must be defined in shared_dict_gba.h
 * Post:function call complete.
 * Rtn: SUCCESS if opcode found and completed, error if opcode not present or other problem.
 */
uint8_t gba_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata )
{

#define	RD_LEN	0
#define	RD0	1
#define	RD1	2

#define	BYTE_LEN 1
#define	HWORD_LEN 2
	
	switch (opcode) { 
//		//no return value:
		case GBA_WR:	
			gba_wr( operand, miscdata );
			break;

		//8bit return values:
		case GBA_RD:
			rdata[RD_LEN] = BYTE_LEN;
			rdata[RD0] = gba_rd( operand );
			break;
		default:
			 //macro doesn't exist
			 return ERR_UNKN_GBA_OPCODE;
	}
	
	return SUCCESS;

}

uint8_t	gba_rd( uint16_t addr )
{
	return 0xAA;
}


void	gba_wr( uint16_t addr, uint8_t data )
{
	return;
}

#endif //GB_CONN
