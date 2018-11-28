#ifndef _fwupdate_h
#define _fwupdate_h


//include target chip port definition library files
#include <stm32f0xx.h>

//#define FWUPDATE __attribute__ ((section (".fw_update")))
#define FWUPDATE __attribute__ ((section (".fw_update"), noinline, noclone))


void erase_main();


#endif
