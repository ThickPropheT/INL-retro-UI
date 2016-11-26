#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <libusb.h>

//uncomment to DEBUG this file alone
#define DEBUG
//"make debug" to get DEBUG msgs on entire program
#include "dbg.h"

#include "usb_operations.h"
#include "write_operations.h"
#include "erase.h"
#include "shared_dictionaries.h"


int main(int argc, char *argv[]) 
{	
        check(argc > 1, "USAGE: inlretro <action o/f>");

	int o_flag = 0;	//LED ON
	int f_flag = 0;	//LED OFF
	int e_flag = 0; //ERASE
	int m_flag = 0; //MIRROR
	char *w_value = NULL; //WRITE FILE
	char *d_value = NULL; //DUMP FILE
	char *s_value = NULL; //SAVE FILE
	char *i_value = NULL; //iNES MAPPER
	char *b_value = NULL; //BOARD   

	int index = 0;
	int rv = 0;
	opterr = 0;

	//getopt returns args till done then returns -1
	//string of possible args : denotes 1 required additional arg
	//:: denotes optional additional arg follows	
	while( (rv = getopt( argc, argv, "ofemw:d:s:i:b:")) != -1) {
		
		switch(rv) {
			case 'o': o_flag = 1; break;
			case 'f': f_flag = 1; break;
			case 'e': e_flag = 1; break;
			case 'm': m_flag = 1; break;
			case 'w': w_value = optarg; break;
			case 'd': d_value = optarg; break;
			case 's': s_value = optarg; break;
			case 'i': i_value = optarg; break;
			case 'b': b_value = optarg; break;
			case '?':
				if ( optopt == 'w' ) {
					log_err("Option -%c requires an argument.", optopt);
				} else if ( optopt == 'd' ) {
					log_err("Option -%c requires an argument.", optopt);
				} else if ( optopt == 's' ) {
					log_err("Option -%c requires an argument.", optopt);
				} else if ( optopt == 'i' ) {
					log_err("Option -%c requires an argument.", optopt);
				} else if ( optopt == 'b' ) {
					log_err("Option -%c requires an argument.", optopt);
				} else if ( isprint(optopt)) {
					log_err("Unknown option -%c .", optopt);
				} else {
					log_err("Unknown option character '\\x%x'", optopt);
				}
				log_err("Improper arguements passed");
				break;
			default:
				sentinel("getopt failed to catch all arg cases");
		}
	
	}
	debug("flags= o:%d f:%d e:%d m:%d", o_flag, f_flag, e_flag, m_flag); 
	debug("args= w:%s d:%s s:%s i:%s b:%s", w_value, d_value, s_value, i_value, b_value); 
	for( index = optind; index < argc; index++) {
		log_err("Non-option arguement: %s \n", argv[index]);
	}


	//context set to NULL since only acting as single user of libusb
	libusb_context *context = NULL;


	//create USBtransfer struct to hold all transfer info
	USBtransfer *transfer = malloc( sizeof(USBtransfer));	
	check_mem(transfer);

	//create usb device handle pointer to interact with retro-prog
	//libusb_device_handle *rprog_handle = NULL;
	transfer->handle = NULL;

	
	//open INL retro prog with firmware version 2.0 or greater
	transfer->handle = open_usb_device( context );
	check( transfer->handle != NULL, "Unable to open INL retro-prog usb device handle.");

	int xfr_cnt = 0;
	uint8_t rbuf[8];

	int i;

	if (e_flag) erase_nes( transfer );

	//handle simple LED ON/OFF within main for now
	if (o_flag | f_flag) {
		printf("before return buffer: ");
		for (i = 0; i < 8; i++) {
			rbuf[i] = 7;
			printf("%x ", rbuf[i]);
		}
		printf("\n");
		transfer->endpoint = USB_IN;
		transfer->request = PINPORT;
		if (o_flag) transfer->wValueLSB = LED_ON;
		if (f_flag) transfer->wValueLSB = LED_OFF;
		transfer->data = rbuf;
		transfer->wLength = 1;

		//send command
		xfr_cnt = usb_transfer( transfer );

		printf("total bytes xfrd: %d \n", xfr_cnt);
		printf("after buffer: ");
		for (i = 0; i < 8; i++) {
			printf("%x ", rbuf[i]);
		}
		printf("\n");
	}



	//if (o_flag) { //ON send REQ_LED_ON
	//	xfr_cnt = usb_transfer( transfer.handle, 
	//		REQ_LED_ON, 
	//		(unsigned char *)buffer254, sizeof(buffer254) );
	//	printf("total bytes xfrd: %d \n", xfr_cnt);
	//}
	//if (f_flag) { //OFF send REQ_LED_OFF
	//	xfr_cnt = usb_write_to_device( transfer.handle, 
	//		REQ_LED_OFF, (unsigned char *)buffer254, sizeof(buffer254) );
	//	printf("total bytes xfrd: %d \n", xfr_cnt);
	//}

	//if (w_value) { //OFF send REQ_LED_OFF
	//	check( write_file( transfer.handle, w_value, i_value, b_value) == SUCCESS, 
	//		"Failed to write file: %s", w_value); 
	//}


	close_usb( context, transfer->handle);	
	
	return 0;

error:
	printf("main program went to error\n");

	return 1;

}
