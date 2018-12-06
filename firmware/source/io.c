#include "io.h"

#ifdef STM_CORE
uint8_t stm_debug_disable = 0;
#endif

//=================================================================================================
//
//	I/O operations
//	This file includes all the io functions possible to be called from the io dictionary.
//
//	See description of the commands contained here in shared/shared_dictionaries.h
//
//=================================================================================================

/* Desc:Function takes an opcode which was transmitted via USB
 * 	then decodes it to call designated function.
 * 	shared_dict_io.h is used in both host and fw to ensure opcodes/names align
 * Pre: Macros must be defined in firmware pinport.h
 * 	opcode must be defined in shared_dict_io.h
 * Post:function call complete.
 * Rtn: SUCCESS if opcode found, error if opcode not present or other problem.
 */
uint8_t io_call( uint8_t opcode, uint8_t miscdata, uint16_t operand, uint8_t *rdata )
{
#define	RD_LEN	0
#define	RD0	1
#define	RD1	2

#define	BYTE_LEN 1
#define	HWORD_LEN 2
	switch (opcode) { 
		case IO_RESET:	
			#ifdef STM_CORE
			//operand sets whether to disable SWCLK/SWDIO on subequent io_reset calls
			stm_debug_disable = operand;
			#endif
				io_reset();			 break;
		#ifdef NES_CONN
		case NES_INIT:	nes_init();			break;
		#endif
		#ifdef SNES_CONN
		case SNES_INIT:	snes_init();			break;
		#endif
		#ifdef GB_CONN
		case GB_POWER_3V:	
				GBP_OP(); GBP_3V(); 		break;
		case GB_POWER_5V:	
				GBP_OP(); GBP_5V(); 		break;
		case GAMEBOY_INIT:	gameboy_init();		break;
//		case GBA_INIT:	gba_init();			break;
		#endif
		#ifdef SEGA_CONN
		case SEGA_INIT:	sega_init();			break;
		#endif
		#ifdef N64_CONN
//		case N64_INIT:	n64_init();			break;
		#endif
		case SWIM_INIT:	
			return swim_init(operand);		break;
		case JTAG_INIT:	
			return jtag_init(operand);		break;

		case EXP0_PULLUP_TEST:	
			rdata[RD_LEN] = BYTE_LEN;
			rdata[RD0] = exp0_pullup_test();	break;
		default:
			 //opcode doesn't exist
			 return ERR_UNKN_IO_OPCODE;
	}
	
	return SUCCESS;

}


//pullup as many cart pins as possible
//goal to be safe state for all hardware
//doesn't currently contain any checks to report error/success from
//this is intended to be the "reset" safest condition for the kazzo
void io_reset()
{

#ifdef STM_CORE
	//reset GPIO perif blocks to default state
	RCC->AHBRSTR |=  ( RCC_AHBRSTR_GPIOARST | RCC_AHBRSTR_GPIOBRST | RCC_AHBRSTR_GPIOCRST | RCC_AHBRSTR_GPIODRST | RCC_AHBRSTR_GPIOFRST );
	RCC->AHBRSTR &= ~( RCC_AHBRSTR_GPIOARST | RCC_AHBRSTR_GPIOBRST | RCC_AHBRSTR_GPIOCRST | RCC_AHBRSTR_GPIODRST | RCC_AHBRSTR_GPIOFRST );
#endif


	//enable the GPIO blocks before can access them
	CTL_ENABLE();

#ifdef STM_CORE
	if (stm_debug_disable == DISABLE_STM_DEBUG){
	//make the SWC/SWD pins like other GPIO
	GPIOA->MODER = 0;
	GPIOA->OSPEEDR = 0;
	GPIOA->PUPDR = 0;
	}
#endif

#ifndef C16nodef
	//First set gameboy/GBA power, default to 3v (safe for both)
	//pull up the GB power control pin
	//prevents conflicts, but also pulls up the 
	//Pmosfet gate which turns off the mosfet
	//leaving the 3v power diode to supply the gameboy cart
	GBP_IP_PU();	
	//If there's a load on the gameboy cart power there will be ~3v present.  
	//If no load/cart, the voltage will will float up to 5v because of Rds_off of the mosfet.
#endif

	//pull up control port
	M2_IP_PU();
	ROMSEL_IP_PU();
	PRGRW_IP_PU();
	CSRD_IP_PU();
	CSWR_IP_PU();
	CICE_IP_PU();
	IRQ_IP_PU();
	CIA10_IP_PU();
#ifndef C3nodef
	FREE_IP_PU();
#endif

	//pull up on FF /OE should disable FF ouput
#ifndef C7nodef
	AHL_IP_PU();
#endif
#ifndef C13nodef
	EXP_DISABLE();
#endif
#ifndef C19nodef
	AFL_IP_PU();
#endif

	//EXP0 input no pullup
	//Lots of possibilities, ~safe bet it will have it's own pull-up/down if needed. 
	//SNES /RESET pin disables SRAM on first few pcb versions
	//NES PRG-ROM /OE (with pulldown) on old INL-ROM v1 boards w/pulldown
	//NED PRG-ROM /WE (with pullup) on INL-ROM v3 boards w/pullup
	//NES CPLD JTAG TDO non-5v tolerant
	//Famicom carts have APU sound (EXP6) shorted to RF sound (EXP0)
	//	-enabling EXP FF output will drive EXP0 to value of A21/EXP6
	EXP0_IP_FL();


	//pull up addr[7:0] bus
	ADDR_ENABLE();
	ADDR_IP();
	ADDR_PU();

	//pull up data bus
	DATA_ENABLE();
	DATA_IP_PU();

	//LED LAST displaying complete..
	//planning to have LED DIM at power on to signify kazzo is in default
	//mostly all pins pulled up state.
	//gives some insight to current state of kazzo since it doesn't reset itself
	//or if kazzo does reset itself due to WDT dim LED can help detect that.
	LED_IP_PU();	//DIM pulled up

}

//NES cartridge interfacing setup
//set outputs as required
//latch address of $0000
//disable NES cart memories
#ifdef NES_CONN
void nes_init() 
{
	//start with a reset
	//expecting user to do this but just to be sure
	io_reset();

	//enable control outputs and disable memories
	//PRG-ROM
	ROMSEL_OP();
	ROMSEL_HI();
	//WRAM (and state of m2 during first half of CPU cycle)
	M2_OP();
	M2_LO();
	//CPU RD
	PRGRW_OP();
	PRGRW_HI();

	//other control pins are inputs, leave as IP pullup from reset

	//disable any CHR/VRAM memories with CHR /RD /WR 
	//prior to setting PPU A13 & /A13 which are /CE pins
	//doing this helps ensure data bus is clear before 
	//using it for AHL clocking
	CSRD_OP();
	CSRD_HI();
	CSWR_OP();
	CSWR_HI();

	//memories are now disabled Data bus should be clear
	DATA_ENABLE();
	DATA_IP_PU();


	//now meet conditions to call other macros
	//setup address $0000
	ADDR_ENABLE();
	ADDR_SET(0x0000);

}
#endif

//SNES cartridge interfacing setup
//set outputs as required
//latch address of $00:0000
//disable cart memories
//reset high disables SRAM and puts INL carts in PRGM mode
//Excersize caution calling this while NES/FC cart inserted on old kazzo versions
//probably won't work if FC inserted due to EXP0-EXP6 short due to audio jumper on cart
#ifdef SNES_CONN
void snes_init() 
{
	//start with a reset
	//expecting user to do this but just to be sure
	io_reset();

	//enable control outputs and disable memories
	//ROM
	ROMSEL_OP();
	ROMSEL_HI();
	CSRD_OP();
	CSRD_HI();
	CSWR_OP();
	CSWR_HI();

	//disable SRAM and put cart in PLAY mode
	EXP0_HI();
	EXP0_OP();
	//if SWIM is active, EXP0 must be set to pullup prior to SWIM transfers

	//other control pins are inputs or unused, leave as IP pullup from reset

	//memories are now disabled Data bus should be clear
	DATA_ENABLE();
	DATA_IP_PU();

	//now meet conditions to call other macros
	//setup address $0000
	ADDR_ENABLE();
	ADDR_SET(0x0000);

	//setup HIGH ADDR with bank $00
	HADDR_ENABLE();
	HADDR_SET(0x00);

}
#endif

//GAMEBOY cartridge interfacing setup
//set outputs as required
//latch address of $0000
//disable cart memories
#ifdef GB_CONN
void gameboy_init() 
{
	//start with a reset
	//expecting user to do this but just to be sure
	io_reset();

	//enable control outputs and disable memories
	//ROM-RAM
	ROMSEL_OP();
	ROMSEL_HI();	//gameboy pin 5 "SRAM /CS"
	CSRD_OP();
	CSRD_HI();	//gameboy pin 4 /RD
	CSWR_OP();
	CSWR_HI();	//gameboy pin 3 /WR

	//Set #RESET pin low
	EXP0_OP();
	EXP0_HI();	//gameboy pin 30 "GAMEBOY /RESET" (GBA /CS2)

	//AUDIO IN (from cart) gameboy pin 31
	//if the cart generates audio it will drive this pin
	//this pin is also used for ROM /WE on some carts
	//such carts should have a pullup on this pin though..
	//use "AUDR" ctl pin to access this pin
	
	//CLK is a 1MHz signal from the gameboy
	//INL6 connects this to GPIO PA8, haven't even assigned this in pinport yet..
	//don't think any carts even need it though..
	//This is also the MCO pin from the STM32
	
	//other control pins are inputs or unused, leave as IP pullup from reset

	//memories are now disabled Data bus should be clear
	DATA_ENABLE();
	DATA_IP_PU();

	//now meet conditions to call other macros
	//setup address $0000
	ADDR_ENABLE();
	ADDR_SET(0x0000);

//want to control this separately
//#ifndef C16nodef
//	//set GB/GBA power to 5v
//	GBP_OP();
//	GBP_5V();
//#endif

}
#endif


//GBA cartridge interfacing setup
//set outputs as required
//latch address of $0000
//disable cart memories
#ifdef GB_CONN
void gba_init() 
{
	//start with a reset
	//expecting user to do this but just to be sure
	//this also sets power to 3v
	io_reset();

	//enable control outputs and disable memories
	//ROM
	ROMSEL_OP();
	ROMSEL_HI();
	CSRD_OP();
	CSRD_HI();
	CSWR_OP();
	CSWR_HI();

	//Set #RESET pin low
	EXP0_LO();
	EXP0_OP();
	//if SWIM is active, EXP0 must be set to pullup prior to SWIM transfers

	//other control pins are inputs or unused, leave as IP pullup from reset

	//memories are now disabled Data bus should be clear
	DATA_ENABLE();
	DATA_IP_PU();

	//now meet conditions to call other macros
	//setup address $0000
	ADDR_ENABLE();
	ADDR_SET(0x0000);

	//default is 3v on gameboy/GBA port

}
#endif


//SEGA Genesis/MegaDrive cartridge interfacing setup
//set outputs as required
//latch address of $00:0000
//disable cart memories
#ifdef SEGA_CONN
void sega_init() 
{
	//start with a reset
	//expecting user to do this but just to be sure
	io_reset();

	//enable control outputs and disable memories
	//ROM
	ROMSEL_OP();
	ROMSEL_HI();	// #C_CE
	CSRD_OP();
	CSRD_HI();	// #C_OE
	CSWR_OP();
	CSWR_HI();	// #UDSW
	PRGRW_OP();
	PRGRW_HI();	// #LDSW

	//disable SRAM and put cart in PLAY mode
	EXP0_HI();
	EXP0_OP();
	//if SWIM is active, EXP0 must be set to pullup prior to SWIM transfers

	//other control pins are inputs or unused, leave as IP pullup from reset

	//memories are now disabled Data bus should be clear
	DATA_ENABLE();
	DATA_IP_PU();

	//now meet conditions to call other macros
	//setup address $0000
	ADDR_ENABLE();
	ADDR_SET(0x0000);

	//setup HIGH ADDR with bank $00
	HADDR_ENABLE();
	HADDR_SET(0x00);

}
#endif

//Initialization of SWIM "single wire interface module" communications
//the SWIM pin depends on INL board design.
//dict call must provide the "swim_lane"
//that swim lane will be used for all subsequent communications.
//TODO setup to control SWIM pin as (psuedo) open drain.
//if swim lane is unknown or other problem return error, else return SUCCESS
uint8_t swim_init( uint8_t swim_lane ) 
{
	switch (swim_lane) {
		case SWIM_ON_A0:	//Most NES & Famicom carts
			//Enable A0 pin on EXT PORT
			EXT_A0_ENABLE();
			//set A0 to open drain with pull-up enabled
			A0_IP_PU();
			#ifdef STM_CORE
				A0_OD();	//set type to open drain
				A0_HI();	//set output high (deasserted)
				A0_OP();	//enable as output to have above take effect
			#endif
			swim_pin = A0;	
			swim_base = A0bank;
			//swim_mask = 1<<A0;
			//set set as output high
			//define the swim pin, base, & mask
			break;
		case SWIM_ON_EXP0:	//SNES carts	
			//set to define used by shared_dict_pinport.h 
			//that way we can call pinport_call(opcode, null, swim_pin, null)
			CTL_ENABLE();
			EXP0_IP_PU();	//this enables pullup for stm
			#ifdef STM_CORE
				EXP0_OD();	//set type to open drain
				EXP0_HI();	//set output high (deasserted)
				EXP0_OP();	//enable as output to have above take effect
			#endif
			swim_pin = EXP0_;	
			swim_base = EXP0bank;
			//swim_mask = 1<<EXP0;
			break;
		case SWIM_ON_D0:	//NES/FC carts with CICOprocesor
			break;
		default: 
			return ERR_UNKN_SWIM_LANE;
	}
	return SUCCESS;
}

//Initialization of JTAG communications
//the JTAG pin depends on INL board design.
//dict call must provide the "jtag_lane"
//that jtag lane will be used for all subsequent communications.
//if jtag lane is unknown or other problem return error, else return SUCCESS
uint8_t jtag_init( uint8_t jtag_lane ) 
{
	switch (jtag_lane) {
		case JTAG_ON_EXP0_3:	//Most NES carts


#ifdef STM_INL6
			//set base & masks
			tdo_base = EXP0bank;
			tdo_pin = EXP0;
			tdi_base = D8bank;
			tdi_pin = D8;
			tms_base = D9bank;
			tms_pin = D9;
			tck_base = D10bank;
			tck_pin = D10;
#else
#endif


			//enable GPIO banks
			//EXT_D8_10_ENABLE();	//EXP0 is also on this GPIO bank
			EXP_ENABLE();
			CTL_ENABLE();		//not really needed..


			//initialize PBJE
			jtag_init_pbje();
			break;
		default: 
			return ERR_UNKN_JTAG_LANE;
	}
	return SUCCESS;
}


//Test starts by verifying EXP0 can be driven low, if not, will return one byte of AUX_PIN
//followed by alternating 0xAA, 0x55, 0xAA...
//This test pulls up EXP0 and then reads AUX_PIN 6 times in rapid succession returning error code
//plus 6 bytes of read data.  If pull up works but is just slow, should see that in return data.
//data[0] marks bit where EXP0 resisdes to provide host with bitmask for EXP0
uint8_t	exp0_pullup_test()
{
	uint16_t temp0, temp1, temp2, temp3, temp4, temp5;

	//first verify EXP0 can be driven low
	EXP0_LO();	//sets O/P and low
	EXP0_OP();
	NOP();		//give some time to settle
		
	EXP0_RD(temp0);
	//data[1] = AUX_IN;	//port where EXP0 resides
	EXP0_IP_FL();	//sets I/P w/o pullup

	//if ( (data[1] & data[0]) == data[0]) {
	if (temp0) {
		//EXP0 was high, couldn't drive EXP0 low
		//data[2] = data[4] = data[6] = 0xAA;
		//data[3] = data[5] = 0x55;
		//return this signature as indication EXP0 failed to drive low
		return EXP0_STUCK_HI;
	}

	//Driving EXP0 low was successful, now pullup and read back
	EXP0_IP_PU();
	EXP0_RD(temp0);
	EXP0_RD(temp1);
	NOP();
	EXP0_RD(temp2);	//3cycles
	NOP();
	NOP();
	EXP0_RD(temp3);	//6cycles
	NOP();
	NOP();
	NOP();
	EXP0_RD(temp4);	//10cycles
	NOP();
	NOP();
	NOP();
	NOP();
	EXP0_RD(temp5);	//15cycles

	//return EXP0 to floating
	EXP0_IP_FL();

	//return the number of cycles it took for EXP0 to go high
	if (temp0) return 0;
	if (temp1) return 1;
	if (temp2) return 3;
	if (temp3) return 6;
	if (temp4) return 10;
	if (temp5) return 15;
	else	 return CANNOT_PULLUP_EXP0;

	

}
