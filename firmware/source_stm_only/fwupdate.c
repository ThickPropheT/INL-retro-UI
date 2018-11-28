#include "fwupdate.h"


FWUPDATE void erase_main() 
{
	//usb driver & this code resisdes in first 2KByte of last (0x0800_0800)
	//The smaller STMF070C6 has 32KByte of flash, and larger STMF070RB has 128KByte
	//C6 flash is split into 1KB pages
	//RB flash is split into 2KB pages
	//So the C6 must leave first two pages alone, and RB only the first page
	//But they're both leaving the first 2KByte untouched
	//And erasing the 30KByte that follows
	//For now we're ignoring the extra 96KByte of additional flash that the RB contains

	while(1){
	}
}
