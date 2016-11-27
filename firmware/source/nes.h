#ifndef _nes_h
#define _nes_h

#include <avr/io.h>
#include "pinport.h"
#include "shared_dictionaries.h"
#include "shared_errors.h"

uint8_t nes_opcode_only( uint8_t opcode );
uint8_t nes_opcode_24b_operand( uint8_t opcode, uint8_t addrH, uint8_t addrL, uint8_t data );
uint8_t nes_opcode_16b_operand_8b_return( uint8_t opcode, uint8_t addrH, uint8_t addrL, uint8_t *data );
void	discrete_exp0_prgrom_wr( uint8_t addrH, uint8_t addrL, uint8_t data );
uint8_t	emulate_nes_cpu_rd( uint8_t addrH, uint8_t addrL );

#endif
