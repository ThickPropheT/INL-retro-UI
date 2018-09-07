#ifndef _n64_h
#define _n64_h

#include "pinport.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"

uint8_t n64_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata );

uint8_t	n64_rd( uint16_t addr );
void	n64_wr( uint16_t addr, uint8_t data );

#endif
