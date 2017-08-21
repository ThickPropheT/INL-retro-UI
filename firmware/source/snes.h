#ifndef _snes_h
#define _snes_h

#include "pinport.h"
#include "buffer.h"	//TODO remove this junk when get rid of FALSE
#include "shared_dictionaries.h"
#include "shared_errors.h"

uint8_t snes_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata );
uint8_t	snes_rom_rd( uint16_t addr );
void	snes_rom_wr( uint16_t addr, uint8_t data );
uint8_t snes_rom_page_rd_poll( uint8_t *data, uint8_t addrH, uint8_t first, uint8_t len, uint8_t poll );


#endif
