#ifndef _gba_h
#define _gba_h

#include "pinport.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"

uint8_t gba_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata );

uint8_t	gba_rd( uint16_t addr );
void	gba_wr( uint16_t addr, uint8_t data );

#endif
