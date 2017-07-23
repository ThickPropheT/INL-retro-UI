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
//	---------------------------------------------------------------------------------------
//	CONTROL PORT: 16bits for function
//	C15-0
//	Directionality: All pins are able to be toggled between input/output individually
//	Driver: All pins indiv selectable pull-up or float for input. Output always push pull.
//	Write/Output: Bit access only, no byte/word accesses
//	Read/Input: Bit access only, returned byte/word will be zero if clear, non-zero if set
//	---------------------------------------------------------------------------------------

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
#define PUPDR_FLT	0x00
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

//kaz6 is PB1
//#define LED	(1U)
//#define IOP_LED_EN RCC_AHBENR_GPIOBEN
//#define GPIO_LED   GPIOB
//
////kaz adapter is PC13
//#define LED	(13U)
//#define IOP_LED_EN RCC_AHBENR_GPIOCEN
//#define GPIO_LED   GPIOC
//
////kaz adapter data0 debug is PB8
//#define DEBUG	(8U)
//#define IOP_DEBUG_EN RCC_AHBENR_GPIOBEN
//#define GPIO_DEBUG   GPIOB
//
//#define LED_ON() (GPIO_LED->ODR |= (0x1U<<LED))
//#define LED_OFF() (GPIO_LED->ODR &= ~(0x1U<<LED))
//
//#define DEBUG_HI() (GPIO_DEBUG->ODR |= (0x1U<<DEBUG))
//#define DEBUG_LO() (GPIO_DEBUG->ODR &= ~(0x1U<<DEBUG))

#ifdef STM_INL6

#define IOP_LED_EN RCC_AHBENR_GPIOBEN

//C0  "ROMSEL"	pinA0
#define C0bank GPIOA 
#define C0	(0U)

//C1  "R/W"	pinA1
#define C1bank GPIOA 
#define C1	(1U)

//C2  "/RD"	pinA2
#define C2bank GPIOA 
#define C2	(2U)

//C3  "/WR"	pinA3
#define C3bank GPIOA 
#define C3	(3U)

//C4  "AUDIO_L"	pinA4
#define C4bank GPIOA 
#define C4	(4U)

//C5  "AUDIO_R"	pinA5
#define C5bank GPIOA 
#define C5	(5U)

//C6  "CART_RST" pinA6
#define C6bank GPIOA 
#define C6	(6U)

//C7  "KEY_DIN"	pinA7
#define C7bank GPIOA 
#define C7	(7U)

//C8  "CLK_OUT"	pinA8
#define C8bank GPIOA 
#define C8	(8U)

//C9  "FLIPFLOP" pinB0
#define C9bank GPIOB 
#define C9	(0U)

//C10 "LED" 	pinB1
#define C10bank GPIOB 
#define C10	(1U)

//C11 "KEY_DOUT" pinD2
#define C11bank GPIOD 
#define C11	(2U)

//C12 "BOOTLOAD" not present
//#define C12bank GPIO 
//#define C12	(U)

//C13 "SWDIO"	pinA13
#define C13bank GPIOA 
#define C13	(13U)

//C14 "SWCLK"	pinA14
#define C14bank GPIOA 
#define C14	(14U)

//C15 "IRQ"	pinA15
#define C15bank GPIOA 
#define C15	(15U)


#endif

#ifdef STM_ADAPTER

#define IOP_LED_EN RCC_AHBENR_GPIOCEN

//C0  "ROMSEL"	pinA4
#define C0bank GPIOA 
#define C0	(4U)

//C1  "R/W"	pinA5
#define C1bank GPIOA 
#define C1	(5U)

//C2  "/RD"	pinA7
#define C2bank GPIOA 
#define C2	(7U)

//C3  "/WR"	pinB0
#define C3bank GPIOB 
#define C3	(0U)

//C4  "AUDIO_L/AHL" pinB1
#define C4bank GPIOB 
#define C4	(1U)

//C5  "AUDIO_R/FREE" pinA6
#define C5bank GPIOA 
#define C5	(6U)

//C6  "CART_RST/EXP0" pinA0
#define C6bank GPIOA 
#define C6	(0U)

////C7  "KEY_DIN"	pinA7
//#define C7bank GPIOA 
//#define C7	(7U)

//C8  "CLK_OUT/M2" pinA3
#define C8bank GPIOA 
#define C8	(3U)

//C9  "FLIPFLOP/AXL" pinA2
#define C9bank GPIOA 
#define C9	(2U)

//C10 "LED" 	pinC13
#define C10bank GPIOC 
#define C10	(13U)

//C11 "KEY_DOUT/CIRAMCE" pinA10
#define C11bank GPIOA 
#define C11	(10U)

//C12 "BOOTLOAD" not present
//#define C12bank GPIO 
//#define C12	(U)

//C13 "SWDIO/CIRAMA10"	pinA13
#define C13bank GPIOA 
#define C13	(13U)

//C14 "SWCLK"	pinA14
#define C14bank GPIOA 
#define C14	(14U)

//C15 "IRQ"	pinA15
#define C15bank GPIOA 
#define C15	(15U)

#endif


#ifdef AVR_KAZZO

//C0  "ROMSEL"	pinC1
#define C0bank GPIOC 
#define C0	(1U)

//C1  "R/W"	pinC2
#define C1bank GPIOC 
#define C1	(2U)

//C2  "/RD"	pinC4
#define C2bank GPIOC 
#define C2	(4U)

//C3  "/WR"	pinC5
#define C3bank GPIOC 
#define C3	(5U)

//C4  "AUDIO_L/AHL" pinC7
#define C4bank GPIOC 
#define C4	(7U)

//C5  "AUDIO_R/FREE" pinC3
#define C5bank GPIOC 
#define C5	(3U)

//C6  "CART_RST/EXP0" pinD0
#define C6bank GPIOD 
#define C6	(0U)

////C7  "KEY_DIN"	pinA7
//#define C7bank GPIOA 
//#define C7	(7U)

//C8  "CLK_OUT/M2" pinC0
#define C8bank GPIOC 
#define C8	(0U)

//C9  "FLIPFLOP/AXL" pinD7
#define C9bank GPIOD 
#define C9	(7U)

//C10 "LED" 	pinD1
#define C10bank GPIOD 
#define C10	(1U)

//C11 "KEY_DOUT/CIRAMCE" pinC6
#define C11bank GPIOC 
#define C11	(6U)

//C12 "BOOTLOAD" pinD6
#define C12bank GPIOD 
#define C12	(6U)

//C13 "SWDIO/CIRAMA10"	pinD5
#define C13bank GPIOD 
#define C13	(5U)

////C14 "SWCLK"	pinA14
//#define C14bank GPIOA 
//#define C14	(14U)

//C15 "IRQ"	pinD3
#define C15bank GPIOD 
#define C15	(3U)

#endif


//pinport abstraction layer pin mappings roughly based on inl6 schematic

//C0  "ROMSEL"
//C1  "R/W"
//C2  "/RD"
//C3  "/WR"
//C4  "AUDIO_L"
//C5  "AUDIO_R"
//C6  "CART_RST"
//C7  "KEY_DIN"
//C8  "CLK_OUT"
//C9  "FLIPFLOP"
//C10 "LED"
//C11 "KEY_DOUT"
//C12 "BOOTLOAD"
//C13 "SWDIO"
//C14 "SWCLK"
//C15 "IRQ"

#define LED 	C10
#define LEDbank	C10bank

#ifdef STM_CORE


//  00- Input (default reset state, except SWC-PA15 & SWD-PA13 default AF)
//  01- Gen Purp Output
//  Assume MODER[1] is clear (default setting, only gets set of AF)
//  Assume PUPDR[1] is clear (default setting, only set if pulldown enabled)
#define PCb_IP_PU(bank, pin)		bank->MODER &= ~(MODER_OP<<(pin*2)); bank->PUPDR |=  (PUPDR_PU<<(pin*2))
#define PCb_IP_FLT(bank, pin)		bank->MODER &= ~(MODER_OP<<(pin*2)); bank->PUPDR &= ~(PUPDR_PU<<(pin*2))
#define PCb_OP_EN(bank, pin)		bank->MODER |=  (MODER_OP<<(pin*2))
#define PCb_READ(bank, pin, val)	val = (bank->IDR & (1<<pin))
#define PCb_SET_HI(bank, pin)		bank->BSRR = 1<<pin
//      PCb_SET_HI(C0bank, C0)
#define PCb_SET_LO(bank, pin)		bank->BRR = 1<<pin 

#endif //STM_CORE

#ifdef AVR_CORE


#define PCb_IP_PU(bank, pin)		bank->DDR &= ~(1<<pin); bank->PORT |=  (1<<pin)
#define PCb_IP_FLT(bank, pin)		bank->DDR &= ~(1<<pin); bank->PORT &= ~(1<<pin)
#define PCb_OP_EN(bank, pin)		bank->DDR |=  (1<<pin)	
#define PCb_READ(bank, pin, val)	val = (bank->PIN & (1<<pin))
#define PCb_SET_HI(bank, pin)		bank->PORT |=  (1<<pin)
#define PCb_SET_LO(bank, pin)		bank->PORT &= ~(1<<pin)

#endif	//AVR_CORE


#endif
