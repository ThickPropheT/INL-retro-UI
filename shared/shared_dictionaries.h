#ifndef _shared_dictionaries_h
#define _shared_dictionaries_h

//list of dictionary reference numbers
//these numbers literally sent in usb control transfer in request field
//the included dictionaries define opcodes and operands contained in transfer wValue wIndex fields
//they also define expected data buffer sizes and contents.
//
//TODO eventually the host code should have access to these libraries during run time.
//that way character strings can be interpreted by parsing the dictionary .h file
//this would also allow for dictionary expansion after compilation of host code.
//However the kazzo fw would still need rebuilt to support dictionary expansion.
//Possible to take this one step further and remove dictionaries from host compiliation.
//that way you simply point the host to a dictionary directory at runtime.
//Perhaps utilizing a database of some sort would be better than directory of text/.h files.
//but since firmware build relies on dictionary definition at build time, perhaps the simplest
//solution of using those avr build .h files slurped up by host at run time is safest and easiest.
//having host capability to convert command string to the usb dict/opcode is the first
//step in having scripting support on host side.  The thought above just expands it one
//step further making the dictionaries themselves operate as run time 'scripts'.

//don't define dictionary #0 as it is common to forget to define 

//=============================================================================================
//=============================================================================================
#define PINPORT 1
#include "shared_dict_pinport.h"
//pinport dictionary has various commands giving low and mid level access to retro prog's i/o pins.
//It also contains internal avr registers associated with the avr's io.
//Access to other internal avr registers should be placed in other associated dictionaries.
//The opcodes in this dictionary should not have any cyclic effect such as pulsing /ROMSEL
//low to read data and then disabling taking /ROMSEL high again.  These commands are intended
//to appear as a single change/edge to cartridge hardware.  Only potential exception to this
//is AHL/AXL clocking which is used to latch values to FF's, that effectively is only one
//state change for the cartridge hardware.
//
//	Many of the opcodes in the second half of this dictionary have the following rules:
//
//	The opcodes that follow operate under some rules that you must adhere to if calling
//	1) Data bus should be free and clear when possible
//		-DATA_IP() is default state
//		-Be cognizant if you're driving the data bus
//			many of these opcodes use the data bus to function.
//		-Many of these opcodes will end up driving the data bus
//			know when that'll happen and free bus when data retreived
//		
//		-Flipflops must be initialized
//			this primarily means CLK pin must be OP and LO ready for CLK command
//		-output of FF must be enabled to actually feed latched value on cart
//			final pcb version will enable EXP FF after clocking.
//			early pcb versions have FF /OE on separate pin not so automatic.
//
//		-control pins must be initialized
//		-enable OP on pins necessary to perform desire of command
//			ie M2 and /ROMSEL must be OP if you're trying to change them with a command.
//
//		-be cognizant of what pins are inputs and which are outputs
//			ie driving PPU /A13 will be fed back to CIRAM /CE so it needs to be IP
//		-if in doubt, leave it as input with pull up, atleast that shouldn't break anything
//
//		-ADDR_OP is default state, these opcodes assume it to be set as it shouldn't conflict
//		-/ROMSEL & M2 expected to be set as outputs
//
//
//=============================================================================================
//=============================================================================================


//=============================================================================================
//=============================================================================================
#define IO 2
#include "shared_dict_io.h"
//io dictionary contains commands 
//Scope of functions contained is intended to be general and generic not specific
//to the cartridge inserted.  The closest these operations get to being cart/system
//specific is in setup for a system.  Calling the cart/system setup contained here
//prepares kazzo for system specific commands.  Once complete with system specifc
//commands come back here to 'deinitialize' access to that cartridge.
//commands in this dictionary are meant to estabilish baseline rules of i/o to 
//support calling higher level system/cart specific functions.
//=============================================================================================
//=============================================================================================


//=============================================================================================
//=============================================================================================
#define NES 3
#include "shared_dict_nes.h"
//nes dictionary contains commands 
//These commands rely on io initialization from io dictionary prior to calling
//This library is intended to contain all NES related opcodes/commands
//=============================================================================================
//=============================================================================================

#endif
