#Single Makefile for all devices

all:
	make -f Make_avr clean hex
	make -f Make_stm clean 
	make -f Make_stm 

avr:
	make -f Make_avr clean program
stm:
	make -f Make_stm clean program
