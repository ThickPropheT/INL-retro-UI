#ifndef _pinport_al_h
#define _pinport_al_h

//Define the board type in makefile
//#define AVR_KAZZO
//#define STM_ADAPTER
//#define STM_INL6 

#ifdef AVR_CORE
	#include "avr_gpio.h"
#elif STM_CORE
	#include <stm32f0xx.h>
#endif

//=============================================================================================
//
// PINPORT ABSTRACTION LAYER
// 
//=============================================================================================
//
//	The pinport abstraction layer creates definitions of generic device GPIO ports.
//	These generic ports are designed to make high level firmware generic to the 
//	actual underlying hardware.  The ports created have defined sizes and access rules
//	that keeps firmware from growing in complexity based on the specific hardware.
//	
//	Idea is to define ports which are available on all hardware, but also create ports
//	which may not be available to other hardwares.  If the firmware is instructed to 
//	perform an operation on a port that's not present, or not initialized/setup error
//	codes will be sent back to the host to designate the lack of port presence.
//

// STM32 GPIO registers are quite different than AVR style
// they are more flexible/capable, but a little tricky to interface with
// some features present on STM32 pins, but not AVR
//  - PULL-DOWN ability (and PULL-UP like AVR)
//  - Speed/Slew rate selection
//  - Open drain outputs (and push/pull like AVR)
//  - Bit set/reset registers to remove necessity of RMW operations
//  - Lockability to keep port config bits from changing until reset
//  - GPIO blocks must be provided a clock and enabled to "turn on"
//    failing to do so will cause hard faults when accessing registers!!
//
//  All GPIO registers can be accessed in byte, halfword, or 32bit full words
//  	unless otherwise noted
//
//  GPIOx->MODER[1:0] 32bit registers control direction/type of driver
//  00- Input (default reset state, except SWC-PA15 & SWD-PA13 default AF)
//  01- Gen Purp Output
//  10- Alternate func (SPI, I2C, etc)
//  11- reserved
//  MODER[1] typically leave clear unless using AltFunc
//  MODER[0] becomes equivalent of AVR DDR
#define MODER_OP	0x01
#define MODER_OP_ALL	0x55555555
#define MODER_AF	0x10
//
//  GPIOx->OTYPER 16bit registers control driver type
//  0- Push Pull (default reset state)
//  1- Open Drain
//  N/A when MODER is set to "00" INPUT
//  we can generally just ignore this register and use pushpull as AVR does
//
//  GPIOx->OSPEEDR[1:0] 32bit registers control pin driver speed/slew
//  x0- Low speed (default reset state, except SWD-PA13 default High Spd) 
//  01- Medium speed
//  11- High speed
//  N/A when MODER is set to "00" INPUT
//  we can generally just ignore this register and use slow speed
//
//  GPIOx->PUPDR[1:0] 32bit registers control pull-up/down resistors
//  this register is in effect even when alternate functions are enabled
//  00- floating/disabled, no pull-up/down  (default for most pins except below)
//  01- Pull-up enabled			    (default SWD-PA13)	also CIRAM A10	
//  10- Pull-down enabled		    (default SWC-PA14)
//  11- Reserved, don't use (prob enables both which would be bad...)
//  PUPDR[0] is kinda like AVR PORT when DDR is set to INPUT, and PUPDR[1]=0
//  This ends up being a little cleaner than AVR i/o interfacing
//  Can probably just enable pull-ups on everything and leave it like that
//  -only exception being USB pins (PA11/12) better leave those floating
//#define PUPDR_FLT	0x00
#define PUPDR_PU	0x01
#define PUPDR_PD	0x10
//
//  GPIOx->IDR 16bit register used to read current input level on pin
//  	this register is read only
//
//  GPIOx->ODR 16bit register used to set output of pin if enabled by MODER
//  	this register is read/writeable
//
//  GPIOx->BSRR 32bit register to only set/clear pins
//  	BR[31:16] upper halfword is will reset/clear pin if written to '1' value
//  	BS[15:00] lower halfword is will set pin if written to '1' value
//  	writing 0 to any bit has no effect
//  	if setting both BS register has priority (bit will be set)
//  	this register is write only!
//
//  GPIO->BRR 16bit register equivalent to upper word of BSRR register above
//      provides convinent separate BR register that doesn't need shifted
//
//  GPIOx->LCKR 17bit register MUST BE ACCESSED in 32bit full words!!!
//	complex sequence needed to set, but once done lock config/altfunc
//	bits for that GPIO.  I interpret this to mean the registers above
//	plus AF registers, exception of IDR, ODR, BSRR registers
//	Good to use this for things than we don't want to accidentally change:
//	 - USB & XTAL pins come to mind as good candidates
//
//  GPIOx->AFRL/H 2 sets of 32bit registers to determine alternate function
//  	of GPIO if enabled with MODER registers. Default is AF0 at reset


// AVR GPIO are rather simple in comparison to STM32 GPIO:
//
// DDRx 8bit direction register
// 	Determines 'direction' of pin driver
// 	set for output, clear of input
// 	reads give current status of register
// 
// PORTx 8bit output/pullup register
// 	writes set/clear pin if DDR is set/OP
// 	write '1' to enable pull-up if DDR is clear/IP
// 	write '0' to disable pull-up if DDR is clear/IP
// 	reads give current status of register
// 	bit access must be done by means of RMW/shadow reg
//
// PINx 8bit read register
// 	read when DDR is clear/IP to get current pin level
// 	reading when O/P is of little use, as should be value of PORTx
// 	read only register




//	---------------------------------------------------------------------------------------
//	CONTROL PORT 
//	
//	This port is present on all devices, however not all pins are present depending on device
//	Keeping this port as bit access only makes it so there is no limit to number of pins.
//	Directionality: All pins are able to be toggled between input/output individually
//	Driver: All pins indiv selectable pull-up or float for input. Output always push pull.
//	Write/Output: Bit access only, no byte/word accesses
//	Read/Input: Bit access only, returned byte/word will be zero if clear, non-zero if set
//
//	PORT C pin definitions
//	Don't assign one mcu pin to more than one pin in this port!
//	mcu pin can be assigned in other ports, as initializing 
//	those ports will 'disable' the pin in this port.
//	if the pin has multiple purposes and would like to give
//	it different names based on the cart inserted, just create
//	multiple defines/names for the one PCn pin.
//
//	---------------------------------------------------------------------------------------

#ifdef STM_INL6

	//     PC0  "MCO"	mcupinA8
	#define C0bank 		GPIOA 
	#define C0		(8U)
	
	//     PC1  "ROMSEL"	mcupinA0
	#define C1bank 		GPIOA 
	#define C1		(0U)
	
	//     PC2  "PRGRW"	mcupinA1
	#define C2bank 		GPIOA 
	#define C2		(1U)
	
	//     PC3  "PAXL"
	//     Not defined
	
	//     PC4  "CSRD"	mcupinA2
	#define C4bank 		GPIOA 
	#define C4		(2U)
	
	//     PC5  "CSWR"	mcupinA3
	#define C5bank 		GPIOA 
	#define C5		(3U)

	//     PC6  "CICE" 	mcupinA10
	#define C6bank 		GPIOA 
	#define C6		(10U)

	//     PC7  "AHL"
	//     Not defined

	//     PC8  "EXP0" 	mcupinA6
	#define C8bank 		GPIOA 
	#define C8		(6U)
	
	//     PC9  "LED" 	mcupinB1
	#define C9bank 		GPIOB 
	#define C9		(1U)

	//     PC10 "IRQ"	mcupinA15
	#define C10bank 	GPIOA 
	#define C10		(15U)

	//     PC11 "CIA10" 	mcupinA9
	#define C11bank 	GPIOA 
	#define C11		(9U)

	//     PC12 "BL" 
	//     Not defined

	//     PC13 "AXL"
	//     Not defined

	//     PC14 "AUDL"	mcupinA4
	#define C14bank 	GPIOA 
	#define C14		(4U)
	
	//     PC15 "AUDR"	mcupinA5
	#define C15bank 	GPIOA 
	#define C15		(5U)

	//     PC16 "CIN" 	mcupinA7
	#define C16bank 	GPIOA 
	#define C16		(7U)

	//     PC17 "SWD" 	mcupinA13
	#define C17bank 	GPIOA 
	#define C17		(13U)
	
	//     PC18 "SWC" 	mcupinA14
	#define C18bank 	GPIOA 
	#define C18		(14U)
	
	//     PC19 "AFL" 	mcupinB0
	#define C19bank 	GPIOB 
	#define C19		(0U)

	//     PC20 "COUT" 	mcupinD2
	#define C20bank 	GPIOD 
	#define C20		(2U)

/* NEED MORE UNIQUE names for these pins to not conflict with Data port definitions...
	//     PC21 "D8" 	mcupinB10
	#define C21bank 	GPIOB 
	#define C21		(10U)

	//     PC22 "D9" 	mcupinB11
	#define C22bank 	GPIOB 
	#define C22		(11U)

	//     PC23 "D10" 	mcupinB12
	#define C23bank 	GPIOB 
	#define C23		(12U)

	//     PC24 "D11" 	mcupinB13
	#define C24bank 	GPIOB 
	#define C24		(13U)

	//     PC25 "D12" 	mcupinB14
	#define C25bank 	GPIOB 
	#define C25		(14U)

	//     PC26 "D13" 	mcupinB15
	#define C26bank 	GPIOB 
	#define C26		(15U)
*/

	#define IOP_LED_EN RCC_AHBENR_GPIOBEN
	
	
	


#endif //STM_INL6

#ifdef STM_ADAPTER

	//     PC0  "MCO"	mcupinA3
	#define C0bank 		GPIOA 
	#define C0		(3U)
	
	//     PC1  "ROMSEL"	mcupinA4
	#define C1bank 		GPIOA 
	#define C1		(4U)
	
	//     PC2  "PRGRW"	mcupinA5
	#define C2bank 		GPIOA 
	#define C2		(5U)
	
	//     PC3  "PAXL"
	//     Not defined
	
	//     PC4  "CSRD"	mcupinA7
	#define C4bank 		GPIOA 
	#define C4		(7U)
	
	//     PC5  "CSWR"	mcupinB0
	#define C5bank 		GPIOB 
	#define C5		(0U)

	//     PC6  "CICE" 	mcupinA10
	#define C6bank 		GPIOA 
	#define C6		(10U)

	//     PC7  "AHL"	mcupinB1
	#define C7bank 		GPIOB 
	#define C7		(1U)

	//     PC8  "EXP0" 	mcupinA0
	#define C8bank 		GPIOA 
	#define C8		(0U)
	
	//     PC9  "LED" 	mcupinC13
	#define C9bank 		GPIOC 
	#define C9		(13U)

	//     PC10 "IRQ"	mcupinA15
	#define C10bank 	GPIOA 
	#define C10		(15U)

	//     PC11 "CIA10" 	mcupinA13
	#define C11bank 	GPIOA 
	#define C11		(13U)

	//     PC12 "BL" 
	//     Not defined

	//     PC13 "AXL"	mcupinA2
	#define C13bank 	GPIOA 
	#define C13		(2U)

	//     PC14 "AUDL"
	//     Not defined
	
	//     PC15 "AUDR"
	//     Not defined

	//     PC16 "CIN" 
	//     Not defined

	//     PC17 "SWD" 	mcupinA13
	//     Not defined due to shared with CIRAM A10
	
	//     PC18 "SWC" 	mcupinA14
	#define C18bank 	GPIOA 
	#define C18		(14U)
	
	//     PC19 "AFL" 
	//     Not defined

	//     PC20 "COUT" 
	//     Not defined

	//     PC21 "D8" 
	//     Not defined

	//     PC22 "D9" 
	//     Not defined

	//     PC23 "D10" 
	//     Not defined

	//     PC24 "D11" 
	//     Not defined

	//     PC25 "D12" 
	//     Not defined

	//     PC26 "D13" 
	//     Not defined

#define IOP_LED_EN RCC_AHBENR_GPIOCEN

#endif	//STM_ADAPTER


#ifdef AVR_KAZZO

	//     PC0  "MCO"	mcupinC0
	#define C0bank 		GPIOC 
	#define C0		(0U)
	
	//     PC1  "ROMSEL"	mcupinC1
	#define C1bank 		GPIOC 
	#define C1		(1U)
	
	//     PC2  "PRGRW"	mcupinC2
	#define C2bank 		GPIOC 
	#define C2		(2U)
	
	//     PC3  "PAXL"	mcupinC3
	//     Not defined
	
	//     PC4  "CSRD"	mcupinC4
	#define C4bank 		GPIOC 
	#define C4		(4U)
	
	//     PC5  "CSWR"	mcupinC5
	#define C5bank 		GPIOC 
	#define C5		(5U)

	//     PC6  "CICE" 	mcupinC6
	#define C6bank 		GPIOC 
	#define C6		(6U)

	//     PC7  "AHL"	mcupinC7
	#define C7bank 		GPIOC 
	#define C7		(7U)

	//     PC8  "EXP0" 	mcupinD0
	#define C8bank 		GPIOD 
	#define C8		(0U)
	
	//     PC9  "LED" 	mcupinD1
	#define C9bank 		GPIOD 
	#define C9		(1U)

	//     PC10 "IRQ"	mcupinD3
	#define C10bank 	GPIOD 
	#define C10		(3U)

	//     PC11 "CIA10" 	mcupinD5
	#define C11bank 	GPIOD 
	#define C11		(5U)

	//     PC12 "BL"	mcupinD6 
	#define C12bank 	GPIOD 
	#define C12		(6U)

	//     PC13 "AXL"	mcupinD7
	#define C13bank 	GPIOD 
	#define C13		(7U)

	//     PC14 "AUDL"
	//     not defined
	
	//     PC15 "AUDR"
	//     not defined

	//     PC16 "CIN" 
	//     not defined

	//     PC17 "SWD" 
	//     not defined
	
	//     PC18 "SWC" 
	//     not defined
	
	//     PC19 "AFL" 
	//     not defined

	//     PC20 "COUT" 
	//     not defined

	//     PC21 "D8" 
	//     not defined

	//     PC22 "D9" 
	//     not defined

	//     PC23 "D10" 
	//     not defined

	//     PC24 "D11" 
	//     not defined

	//     PC25 "D12" 
	//     not defined

	//     PC26 "D13"
	//     not defined

#endif //AVR_KAZZO


////////////////////////////////////////////////////////////////////////////////
//	PORT C pin mappings
////////////////////////////////////////////////////////////////////////////////
//
//	PC0-13 are defined based on majority of avr kazzos PORTC-PORTD
//	PC0  "MCO"	mcu clock out M2/phi2, Sysclk, etc
#define MCO 		C0
#define MCObank		C0bank
#define M2 		C0
#define M2bank		C0bank

//	PC1  "ROMSEL"	Cartridge rom enable
#define ROMSEL 		C1
#define ROMSELbank	C1bank

//	PC2  "PRGRW"	NES CPU R/W signal
#define PRGRW 		C2
#define PRGRWbank	C2bank

//	PC3  "PAXL"	purple kazzo EXP flipflop latch, FREE on most kazzos
#define PAXL 		C3
#define PAXLbank	C3bank

//	PC4  "CSRD"	NES CHR/SNES /RD
#define CSRD 		C4
#define CSRDbank	C4bank

//	PC5  "CSWR"	NES CHR/SNES /WR
#define CSWR 		C5
#define CSWRbank	C5bank

//	PC6  "CICE" 	NES CIRAM /CE
#define CICE 		C6
#define CICEbank	C6bank

//	PC7  "AHL" 	ADDR HI Latch
#define AHL 		C7
#define AHLbank		C7bank

//	PC8  "EXP0" 	NES EXP0, cart-console /RESET
#define EXP0 		C8
#define EXP0bank	C8bank

//	PC9  "LED" 	kazzos tied this to NES EXP9, INL6 connects to CIC CLK
#define LED 		C9
#define LEDbank		C9bank

//	PC10 "IRQ"	console CPU interrupt from cart
#define IRQ 		C10
#define IRQbank		C10bank

//	PC11 "CIA10" 	NES CIRAM A10
#define CIA10 		C11
#define CIA10bank	C11bank

//	PC12 "BL" 	Bootloader pin 
#define BL 		C12
#define BLbank		C12bank

//	PC13 "AXL" 	EXP FF latch and /OE, purple kazzos this was only /OE
#define AXL 		C13
#define AXLbank		C13bank

//	 INLretro6 adds following pins
//	PC14 "AUDL"	cart audio
#define AUDL 		C14
#define AUDLbank	C14bank

//	PC15 "AUDR"	cart audio
#define AUDR 		C15
#define AUDRbank	C15bank

//	PC16 "CIN"	CIC data in
#define CIN 		C16
#define CINbank		C16bank

//	PC17 "SWD" 	mcu debug
#define SWD 		C17
#define SWDbank		C17bank

//	PC18 "SWC" 	mcu debug
#define SWC 		C18
#define SWCbank		C18bank

//	PC19 "AFL" 	flipflop addr expansion for FF0-7
#define AFL 		C19
#define AFLbank		C19bank

//	PC20 "COUT" CIC data out
#define COUT 		C20
#define COUTbank	C20bank

//	 INLretro6 gains direct control over NES EXP port and is used for N64 control pins:
/*
//	PC21 "D8" 
#define D8 		C21
#define D8bank		C21bank

//	PC22 "D9" 
#define D9 		C22
#define D9bank		C22bank

//	PC23 "D10" 
#define D10 		C23
#define D10bank		C23bank

//	PC24 "D11" 
#define D11 		C24
#define D11bank		C24bank

//	PC25 "D12" 
#define D12 		C25
#define D12bank		C25bank

//	PC26 "D13" 
#define D12 		C26
#define D12bank		C26bank

//	PC27 "D14" 
#define D12 		C27
#define D12bank		C27bank

//		D15 & D16 are defined as CICE/CIA10 above
*/

#ifdef STM_CORE

#define CTL_IP_PU(bank, pin)		bank->MODER &= ~(MODER_OP<<(pin*2)); bank->PUPDR |=  (PUPDR_PU<<(pin*2))
#define CTL_IP_FLT(bank, pin)		bank->MODER &= ~(MODER_OP<<(pin*2)); bank->PUPDR &= ~(PUPDR_PU<<(pin*2))
#define CTL_OP(bank, pin)		bank->MODER |=  (MODER_OP<<(pin*2))
#define CTL_READ(bank, pin, val)	val = (bank->IDR & (1<<pin))
#define CTL_SET_HI(bank, pin)		bank->BSRR = 1<<pin
#define CTL_SET_LO(bank, pin)		bank->BRR = 1<<pin 

#endif //STM_CORE

#ifdef AVR_CORE

#define CTL_IP_PU(bank, pin)		bank->DDR &= ~(1<<pin); bank->PORT |=  (1<<pin)
#define CTL_IP_FLT(bank, pin)		bank->DDR &= ~(1<<pin); bank->PORT &= ~(1<<pin)
#define CTL_OP(bank, pin)		bank->DDR |=  (1<<pin)	
#define CTL_READ(bank, pin, val)	val = (bank->PIN & (1<<pin))
#define CTL_SET_HI(bank, pin)		bank->PORT |=  (1<<pin)
#define CTL_SET_LO(bank, pin)		bank->PORT &= ~(1<<pin)

#endif	//AVR_CORE


// CONTROL PORT MACROS to simplify flipflop operations
//

#define _AHL_CLK()      CTL_SET_HI(AHLbank, AHL); CTL_SET_LO(AHLbank, AHL)
#define _AXL_CLK()      CTL_SET_HI(AXLbank, AXL); CTL_SET_LO(AXLbank, AXL)

//	---------------------------------------------------------------------------------------
//	DATA PORT 8bit
//	
//	This port is present on all devices
//	Directionality: All pins are bidirectional controlled as a whole
//	Driver: All pins are push-pull, and unknown floating/pull-up when input
//		main reason to be unknown is AVR uses PORT for pull-up enable
//		don't want to require re-enabling pullups for every data access
//	Write/Output: Byte access only, no bit accesses.  Must be returned to input after read!
//	Read/Input: Default condition, byte access only
//
//	---------------------------------------------------------------------------------------
//
#ifdef STM_INL6

	//All 8bits are on GPIOB inorder, but mapped to bits9-2 for 5v tolerance
	//I get why I designed it that way so D8-13 could follow in order..
	//But with D8-15 required to be broken up anyways, perhaps it would have
	//made more sense to map D0-7 to bits 9-15 so byte access could be performed
	//without shifting on Data7-0...
	//IDK if I will reroute the board for production or not... Only other way to
	//do it senisbly really makes a mess of the upper byte of Data
	#define Dbank 		GPIOB 

	//IP and OP assume MODER[1] is clear (ie not set to Alt Func)
	#define _DATA_IP()	Dbank->MODER &= ~(MODER_OP_ALL & 0x000FFFF0)
	#define _DATA_OP()	Dbank->MODER |=  (MODER_OP_ALL & 0x000FFFF0)
	#define _DATA_SET(data)	Dbank->ODR = (Dbank->ODR & 0xFC03) | (data<<2)			
	#define _DATA_RD(data)	data = (Dbank->IDR>>2) & 0x00FF

#endif	//STM_INL6

#ifdef STM_ADAPTER

	//All 8bits are on GPIOB inorder, but mapped to bits15-8
	#define Dbank 		GPIOB 

	//IP and OP assume MODER[1] is clear (ie not set to Alt Func)
	#define _DATA_IP()	Dbank->MODER &= ~(MODER_OP_ALL & 0xFFFF0000)
	#define _DATA_OP()	Dbank->MODER |=  (MODER_OP_ALL & 0xFFFF0000)
	//TODO create byte wide port structs to grant byte accesses so doesn't need shifted
	#define _DATA_SET(data)	Dbank->ODR = (Dbank->ODR & 0x00FF) | (data<<8)			
	#define _DATA_RD(data)	data = (Dbank->IDR>>8) & 0x00FF

#endif	//STM_ADAPTER

#ifdef AVR_KAZZO

	//All 8bits are on GPIOB aligned perfectly
	#define Dbank 		GPIOB 

	#define _DATA_IP()      Dbank->DDR = 0x00
	#define _DATA_OP()      Dbank->DDR = 0xFF
	#define _DATA_SET(data) Dbank->PORT = data
	#define _DATA_RD(data)  data = Dbank->PIN


#endif	//AVR_KAZZO


//	---------------------------------------------------------------------------------------
//	ADDRESS PORT 16bit
//	
//	This port is present on all devices
//	Directionality: All pins are forced output 
//	Driver: All pins are push-pull
//	Write/Output: Byte/half word access only, no bit accesses
//	Read/Input: Not supported
//
//	---------------------------------------------------------------------------------------

#ifdef STM_INL6

	//All 16bits are on GPIOC in perfect alignment
	#define Abank 		GPIOC 

	#define _ADDR_IP()	Abank->MODER &= ~MODER_OP_ALL
	#define _ADDR_OP()	Abank->MODER |=  MODER_OP_ALL 
	#define _ADDRL(low)	Abank->ODR = (Abank->ODR & 0xFF00) | low						
	#define _ADDRH(high)	Abank->ODR = (Abank->ODR & 0x00FF) | (high<<8)			
	#define _ADDR(hword)	Abank->ODR = hword

#endif	//STM_INL6

#ifdef STM_ADAPTER

	// A15-8 are behind AHL flipflop
	// A7-6 are on GPIO A9-8 
	// A5-0 are on GPIO A7-2
	#define A76bank 	GPIOA 
	#define A50bank 	GPIOB 

	#define _ADDR_IP()	A76bank->MODER &= ~(MODER_OP_ALL & 0x000F0000); A50bank->MODER &= ~(MODER_OP_ALL & 0x0000FFF0) 
	#define _ADDR_OP()	A76bank->MODER |=  (MODER_OP_ALL & 0x000F0000); A50bank->MODER |=  (MODER_OP_ALL & 0x0000FFF0)  
	#define _ADDRL(low)	A76bank->ODR = (A76bank->ODR & 0xFCFF) | ((low & 0xC0)<<2);A50bank->ODR = (A50bank->ODR & 0xFF03) | ((low & 0x3F)<<2)
//clocks must be initialized, Data bus clear
	#define _ADDRH(high)	_DATA_OP(); _DATA_SET(high); _AHL_CLK(); _DATA_IP();
	#define _ADDR(hword)	ADDRL(hword); _ADDRH(hword>>8)

#endif	//STM_ADAPTER

#ifdef AVR_KAZZO

	// A15-8 are behind AHL flipflop
	// A7-0 are on GPIOA perfectly aligned
	#define _ALbank 	GPIOA 

	#define _ADDRL(low)	GPIOA->PORT = low
//clocks must be initialized, Data bus clear
	#define _ADDRH(high)	_DATA_OP(); _DATA_SET(high); _AHL_CLK(); _DATA_IP();
	#define _ADDR(hword)	ADDRL(hword); _ADDRH(hword>>8)

#endif	//AVR_KAZZO




#endif
