
#include <avr/io.h>
#include "pinport.h"

//Goal is to silience everything
//Clear busses and pull up when able
void io_init()
{
	//pull up data bus
	DATA_IP();
	DATA_HI();

	//pull up addr[7:0] bus
	ADDR_IP();
	ADDR_HI();

	//EXP0 input no pullup
	//SNES /RESET pin disables SRAM on first few pcb versions
	//NES PRG-ROM /OE on old INL-ROM v1 boards w/pulldown
	//NED PRG-ROM /WE on INL-ROM v3 boards w/pullup
	//NES CPLD JTAG TDO non-5v tolerant
	EXP0_IP();
	EXP0_LO();

	//IRQ pullup NES boards will drive this pin
	IRQ_IP();
	IRQ_HI();

	//LED LAST displaying complete..
}
