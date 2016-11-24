#include "io.h"


//pullup as many cart pins as possible
//goal to be safe state for all hardware
void io_pullup()
{
	//pull up addr[7:0] bus
	_ADDR_IP();
	_ADDR_HI();

	//pull up data bus
	_DATA_IP();
	_DATA_HI();

	//pull up control port
	_CTL_IP();
	_CTL_HI();

	//pull up aux port
	_AUX_IP();
	_AUX_HI();
	//Aux port EXP /OE control
	//pull up on EXP FF should disable FF ouput

	//EXP0 input no pullup
	//Lots of possibilities, ~safe bet it will have it's own pull-up/down if needed. 
	//SNES /RESET pin disables SRAM on first few pcb versions
	//NES PRG-ROM /OE (with pulldown) on old INL-ROM v1 boards w/pulldown
	//NED PRG-ROM /WE (with pullup) on INL-ROM v3 boards w/pullup
	//NES CPLD JTAG TDO non-5v tolerant
	_EXP0_FLT();

	//LED LAST displaying complete..
}
