#ifndef _shared_dictionaries_h
#define _shared_dictionaries_h

//list of dictionary reference numbers
//these numbers literally sent in usb control transfer in request field
//the included dictionaries define opcodes and operands contained in transfer wValue wIndex fields
//they also define expected data buffer sizes and contents.


#define PINPORT 1
#include "shared_dict_pinport.h"
//pinport dictionary has various commands giving low and mid level access to retro prog's i/o pins.
//It also contains internal avr registers associated with the avr's io.
//Access to other internal avr registers should be placed in other associated dictionaries.

#endif
