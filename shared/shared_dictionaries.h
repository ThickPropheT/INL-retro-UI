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


//Notes:
//	each unique read/write function in avr takes about 50 bytes...

//don't define dictionary #0 as it is common to forget to define 

//=============================================================================================
//=============================================================================================
#define DICT_PINPORT 1
#include "shared_dict_pinport.h"
//pinport dictionary has various commands giving low and mid level access to retro prog's i/o pins.
//See abstraction layer port definitions in firmware pinport_al.h file for more details.
//An effort has been made to make opcodes in this dictionary hardware independent.
//=============================================================================================
//=============================================================================================


//=============================================================================================
//=============================================================================================
#define DICT_IO 2
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
#define DICT_NES 3
#include "shared_dict_nes.h"
//nes dictionary contains commands 
//These commands rely on io initialization from io dictionary prior to calling
//This library is intended to contain all NES related opcodes/commands
//=============================================================================================
//=============================================================================================


//=============================================================================================
//=============================================================================================
#define DICT_SNES 4
#include "shared_dict_snes.h"
//snes dictionary contains commands 
//These commands rely on io initialization from io dictionary prior to calling
//This library is intended to contain all SNES related opcodes/commands
//=============================================================================================
//=============================================================================================


//=============================================================================================
//=============================================================================================
#define DICT_BUFFER 5
#include "shared_dict_buffer.h"
//mcu buffer dictionary commands 
//This library is intended to contain all buffer related opcodes/commands
//also contains defines for both host and firmware such as buffer status numbers
//=============================================================================================
//=============================================================================================


//=============================================================================================
//=============================================================================================
#define DICT_USB 6
#include "shared_dict_usb.h"
//currently no actual dictionary as there are no opcodes.
//just used to return status of usbfunctions in event of a transfer error. 
//contains definitions of data transactions between host and firmware
//=============================================================================================
//=============================================================================================


//=============================================================================================
//=============================================================================================
#define DICT_OPER 7
#include "shared_dict_operation.h"
//dictionary used to initialize and control operation_info variables
//=============================================================================================
//=============================================================================================
#endif
