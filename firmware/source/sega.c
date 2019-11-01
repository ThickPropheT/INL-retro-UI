#include "sega.h"

//only need this file if connector is present on the device
#ifdef SEGA_CONN 

uint16_t sega_addr = 0;

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

	uint16_t temp;
	
	switch (opcode) { 
//		//no return value:
		case GEN_SET_ADDR:	
			sega_addr = operand;
			ADDR_SET(sega_addr);
			break;
		case GEN_WR_LO:	
			sega_addr = operand;
			gen_wr_lo( operand, miscdata );
			break;
		case GEN_WR_HI:	
			sega_addr = operand;
			gen_wr_hi( operand, miscdata );
			break;

		case GEN_SET_BANK:	
			temp = ADDR_CUR; 	//this will get stomped
#define LOMEM_TIME_MASK 0x84
			//A17-18, 20-23
			FFADDR_SET( operand | LOMEM_TIME_MASK );	//TODO decode #TIME & LO_MEM
			ADDR_SET(temp);		//restore A1-16
#define SEGA_A19_MASK 0x04
			//A19
			if ( operand & SEGA_A19_MASK ) {
				IRQ_HI();
			} else {
				IRQ_LO();
			}
			//use of flip-flop corrupts A1-A16, restore it
			ADDR_SET(sega_addr);
			break;

		case GEN_FLASH_WR_ADDROFF:
			sega_addr += miscdata;
			gen_flash_wr(sega_addr, operand);
			break;

		case GEN_SST_FLASH_WR_ADDROFF:
			sega_addr += miscdata;
			gen_sst_flash_wr(sega_addr, operand);
			break;

		//8bit return values:
		case GEN_ROM_RD:
			sega_addr = operand;
			rdata[RD_LEN] = HWORD_LEN;
			temp = gen_rom_rd( operand );
			rdata[RD0] = temp;
			rdata[RD1] = temp>>8; 
			break;
		default:
			 //macro doesn't exist
			 return ERR_UNKN_SEGA_OPCODE;
	}
	
	return SUCCESS;

}

uint16_t gen_rom_rd( uint16_t addr )
{
	uint16_t rv;
	uint8_t temp;

	ADDR_SET(addr);

	//set #C_CE
	ROMSEL_LO();

	//set #C_OE
	CSRD_LO();

	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	//6 above were working, trying more
	//NOP();
	//NOP();
	//NOP();
	//NOP();
	//NOP();
	//NOP();
	
	
	
	////latch data high byte
	//rv = HDATA_VAL;

	////shift into upper byte
	//rv = rv<<8;

	////latch data low byte
	//DATA_RD(temp);

	//rv |= temp;
	DATA16H_RD(rv);
	rv = rv << 8;
	DATA16L_RD(temp);
	rv |= temp;

	//return bus to default
	CSRD_HI();
	ROMSEL_HI();

	return rv;
}


void	gen_wr_lo( uint16_t addr, uint8_t data )
{

	ADDR_SET(addr);

	//put data on bus
	DATA_OP();
	DATA_SET(data); //lower byte D0-7

	//set #C_CE
	ROMSEL_LO();

	//set #C_OE
	//CSRD_LO();

	//set #LDSW
	PRGRW_LO();	

	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	
	//latch data with /WE - #LDSW
	PRGRW_HI();	

	//return bus to default
	//CSRD_HI();
	ROMSEL_HI();

	//Free data bus
	DATA_IP();
}

//TODO this function is untested, but I think it'll work..
void	gen_wr_hi( uint16_t addr, uint8_t data )
{

	ADDR_SET(addr);

	//put data on bus
	//DATA_OP();
	//DATA_SET(data); //lower byte D0-7
	
	//set data bus to output
//TODO maybe want a function that only sets upper byte to output..?
	DATA16_OP();
	//DATA16L_SET(data);
	DATA16H_SET(data); //put 8bits of data on high byte

	//set #C_CE
	ROMSEL_LO();  //enables level shifter

	//wait for data to get to flash before latching
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();

	//set #UDSW
	CSWR_LO();	

	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	
	//latch data with /WE - #UDSW
	CSWR_HI();	

	NOP();
	NOP();
	NOP();

	//return bus to default
	ROMSEL_HI();

	//Free data bus
	DATA16_IP();
}


void	gen_flash_wr( uint16_t addr, uint16_t data )
{

	uint8_t temp = data;

	ADDR_SET(addr);

	//put data on bus
	//DATA_OP();
	//DATA_SET(temp); //lower byte D0-7
	DATA16_OP();
	DATA16L_SET(data);
	data = data>>8;
	DATA16H_SET(data); //put 8bits of data on high byte

	//TODO figure out why this is needed...
	//guessing macro expansion or something with setting both bytes separately
	DATA_SET(temp); //lower byte D0-7

//TODO set upper byte of data

	//set #C_CE
	ROMSEL_LO();

	//set #C_OE
	//flash writes must be high
	//CSRD_LO();

	//set #LDSW
	//PRGRW_LO();	
	// #UDSW B29  CPU D8-15 data strobe
	CSWR_LO();	

	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	//6 above were working, testing more
	//NOP();
	//NOP();
	//NOP();
	//NOP();
	//NOP();
	//NOP();
	
	//latch data with /WE - #LDSW
	//PRGRW_HI();	
	CSWR_HI();	

	//return bus to default
	ROMSEL_HI();

	//Free data bus
	//DATA_IP();
	DATA16_IP();
}

/* Desc:SEGA GENESIS ROM Page Read with optional USB polling
 * 	/ROMSEL based on romsel arg, EXP0/RESET unaffected
 *	if poll is true calls usbdrv.h usbPoll fuction
 *	this is needed to keep from timing out when double buffering usb data
 * Pre: snes_init() setup of io pins
 *	num_bytes can't exceed 256B page boundary
 * Post:address left on bus
 * 	data bus left clear
 *	data buffer filled starting at first to last
 * Rtn:	Index of last byte read
 */
uint8_t genesis_page_rd( uint8_t *data, uint16_t addrH, uint8_t first, uint8_t len )
{
	uint8_t i;

	uint16_t address = first>>1; //shift because there is no A0

	//address = ((addrH<<8) | first)>>1;	//shift because there is no A0
	address = (addrH<<7) | address;	//shift because there is no A0

	//set address
	//ADDRH(addrH);
	ADDRH(address>>8);
	
	//set #C_CE
	ROMSEL_LO();

	//set #C_OE
	CSRD_LO();

	first = address;

	//set lower address bits
	ADDRL(first);		//doing this prior to entry and right after latching
				//gives longest delay between address out and latching data
	for( i=0; i<=len; i++ ) {

		//gameboy needed some extra NOPS
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		
		//latch data high byte
		data[i] = HDATA_VAL;

		i++;

		//latch data low byte
		DATA_RD(data[i]);

		//set lower address bits
		//ADDRL(++first);	THIS broke things, on stm adapter because macro expands it twice!
		first++;
		ADDRL(first);
	}

	//return bus to default
	CSRD_HI();
	ROMSEL_HI();
	
	//return index of last byte read
	return i;
}

/* Desc:SEGA GENESIS RAM Page Read with optional USB polling
 * 	/ROMSEL based on romsel arg, EXP0/RESET unaffected
 *	if poll is true calls usbdrv.h usbPoll fuction
 *	this is needed to keep from timing out when double buffering usb data
 * Pre: snes_init() setup of io pins
 *	num_bytes can't exceed 256B page boundary
 * Post:address left on bus
 * 	data bus left clear
 *	data buffer filled starting at first to last
 * Rtn:	Index of last byte read
 */
uint8_t genesis_ram_page_rd( uint8_t *data, uint16_t addrH, uint8_t first, uint8_t len )
{
	uint8_t i;

	//set address bus
	ADDRH(addrH);
	
	//set #C_CE
	ROMSEL_LO();

	//set #C_OE
	CSRD_LO();

	//set lower address bits
	ADDRL(first);		//doing this prior to entry and right after latching
				//gives longest delay between address out and latching data

	for( i=0; i<=len; i++ ) {

		//gameboy needed some extra NOPS
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		
		//latch data high byte
	//	data[i] = HDATA_VAL;

	//	i++;

		//latch data low byte
		DATA_RD(data[i]);

		//set lower address bits
		//ADDRL(++first);	THIS broke things, on stm adapter because macro expands it twice!
		first++;
		ADDRL(first);
	}

	//return bus to default
	CSRD_HI();
	ROMSEL_HI();
	
	//return index of last byte read
	return i;
}

void	gen_sst_flash_wr( uint16_t addr, uint16_t data )
{
	uint16_t rv;

//dict.sega("GEN_SET_ADDR", 0x5555)
//dict.sega("GEN_FLASH_WR_ADDROFF", 0x00AA, 0)
	gen_flash_wr(0x5555, 0x00AA);

//dict.sega("GEN_SET_ADDR", 0x2AAA)
//dict.sega("GEN_FLASH_WR_ADDROFF", 0x0055, 0)
	gen_flash_wr(0x2AAA, 0x0055);

//dict.sega("GEN_SET_ADDR", 0x5555)
//dict.sega("GEN_FLASH_WR_ADDROFF", 0x00A0, 0) --write byte command
	gen_flash_wr(0x5555, 0x00A0);

//dict.sega("GEN_SET_ADDR", addr)
//dict.sega("GEN_FLASH_WR_ADDROFF", value, 0) --write value
	gen_flash_wr(addr, data);

	do {
		rv = gen_rom_rd(addr);
	//	usbPoll();	//orignal kazzo needs this frequently to slurp up incoming data
	} while (rv != gen_rom_rd(addr));

	return;
	
}

#endif //SEGA_CONN
