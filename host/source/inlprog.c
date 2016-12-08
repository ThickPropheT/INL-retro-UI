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

#include "shared_dictionaries.h"
#include "usb_operations.h"
#include "write_operations.h"
#include "erase.h"
#include "test.h"
#include "cartridge.h"
#include "file.h"
#include "enums.h"


int main(int argc, char *argv[]) 
{	

	//lower case flags suggested for average user
	int e_flag = 0; //FORCE ERASE
	int f_flag = 0; //FORCE ALL CHECKS PASSING
	int h_flag = 0; //HELP MESSAGE
	int i_flag = 0; //input file is interlaced
	int n_flag = 0;	//LED OFF
	int o_flag = 0;	//LED ON
	int t_flag = 0; //test all SRAMs found
	int x_flag = 0; //disable all auto detecting
	int y_flag = 0; //diable all auto doubling

	char *b_value = NULL; //SUBMAPPER #
	char *c_value = NULL; //CONSOLE NAME
	char *d_value = NULL; //DUMP TO FILE
	char *m_value = NULL; //MAPPER #
	char *p_value = NULL; //PROGRAM FILE
	char *s_value = NULL; //SCRIPT FILE
	char *v_value = NULL; //MAPPER VARIANT


	//upper case flags suggested for ADVANCED users
	int T_flag = 0; //TEST

	char *C_value = NULL; //program chr file
	char *L_value = NULL; //LIBUSB debugging value
	char *K_value = NULL; //connect to kazzo firmware version #
	char *O_value = NULL; //start programming at offset base value
	char *P_value = NULL; //program prg file
	char *S_value = NULL; //program SNES binary file
	char *W_value = NULL; //program WRAM/SRAM file


	int index = 0;
	int rv = 0;
	opterr = 0;

	//getopt returns args till done then returns -1
	//string of possible args : denotes 1 required additional arg
	//:: denotes optional additional arg follows	
	while( (rv = getopt( argc, argv, "onefhitxyTb:c:d:m:p:s:v:C:L:K:O:P:S:W:")) != -1) {
		
		switch(rv) {
			case 'o': o_flag = 1; break;
			case 'n': n_flag = 1; break;
			case 'e': e_flag = 1; break;
			case 'f': f_flag = 1; break;
			case 'h': h_flag = 1; break;
			case 'i': i_flag = 1; break;
			case 't': t_flag = 1; break;
			case 'x': x_flag = 1; break;
			case 'y': y_flag = 1; break;
			case 'T': T_flag = 1; break;

			case 'b': b_value = optarg; break;
			case 'c': c_value = optarg; break;
			case 'd': d_value = optarg; break;
			case 'm': m_value = optarg; break;
			case 'p': p_value = optarg; break;
			case 's': s_value = optarg; break;
			case 'v': v_value = optarg; break;
			case 'C': C_value = optarg; break;
			case 'L': L_value = optarg; break;
			case 'K': K_value = optarg; break;
			case 'O': O_value = optarg; break;
			case 'P': P_value = optarg; break;
			case 'S': S_value = optarg; break;
			case 'W': W_value = optarg; break;

			case '?':
				if(( optopt == 'b' )
				|| ( optopt == 'c' )
				|| ( optopt == 'd' )
				|| ( optopt == 'm' )
				|| ( optopt == 'p' )
				|| ( optopt == 's' )
				|| ( optopt == 'v' )
				|| ( optopt == 'C' )
				|| ( optopt == 'L' )
				|| ( optopt == 'K' )
				|| ( optopt == 'O' )
				|| ( optopt == 'P' )
				|| ( optopt == 'S' )
				|| ( optopt == 'W' )){
					log_err("Option -%c requires an argument.", optopt);
					goto error;

				} else if ( isprint(optopt)) {
					log_err("Unknown option -%c .", optopt);
					goto error;
				} else {
					log_err("Unknown option character '\\x%x'", optopt);
					goto error;
				}
				log_err("Improper arguements passed");
					goto error;
				break;
			default:
				sentinel("getopt failed to catch all arg cases");
		}
	
	}

	debug("flags= o:%d n:%d e:%d f:%d h:%d i:%d t:%d x:%d y:%d T:%d", 
		o_flag, n_flag, e_flag, f_flag, h_flag, i_flag, t_flag, x_flag, y_flag, T_flag ); 
	debug("args= b:%s c:%s d:%s m:%s p:%s", b_value, c_value, d_value, m_value, p_value); 
	debug("args= s:%s v:%s C:%s L:%s K:%s", s_value, v_value, C_value, L_value, K_value); 
	debug("args= O:%s P:%s S:%s W:%s", O_value, P_value, S_value, W_value); 

	for( index = optind; index < argc; index++) {
		log_err("Non-option arguement: %s \n", argv[index]);
	}

	//TODO display better help message 
	if (h_flag) {
		printf("you've asked for help but the help message still needs created...\n");
		return 0;
	}

	//context set to NULL since only acting as single user of libusb
	libusb_context *context = NULL;

	//create USBtransfer struct to hold all transfer info
	USBtransfer *transfer = malloc( sizeof(USBtransfer));	
	//create usb device handle pointer to interact with retro-prog
	//libusb_device_handle *rprog_handle = NULL;
	transfer->handle = NULL;

	//create file object/struct 
	rom_image *rom = malloc( sizeof(rom_image));	
	rom->fileptr = NULL;

	check_mem(transfer);
	check_mem(rom);


	//command line arg L_value to set different LIBUSB debugging options
	//any value > 0 also prints debug statements in open_usb_device function
	int libusb_log = LIBUSB_LOG_LEVEL_NONE;	// 0: default no msgs ever printed
	if (L_value != NULL) {
		check( isdigit(L_value[0]), 
			"Invalid LIBUSB_LOG_LEVEL: %s, only single digit allowed", L_value);
		check( strlen(L_value) == 1, 
			"Invalid LIBUSB_LOG_LEVEL: %s, only single digit allowed", L_value);
		libusb_log = atoi(L_value);
		check( ((libusb_log >= LIBUSB_LOG_LEVEL_NONE) && (libusb_log <=LIBUSB_LOG_LEVEL_DEBUG)), 
		    "Invalid LIBUSB_LOG_LEVEL: %d, must be from 0 to 4", libusb_log );
	}
	//open INL retro prog with firmware version 2.0 or greater
	if (K_value != NULL) {
		//TODO take K_value option to connect to different version kazzo
	}
	transfer->handle = open_usb_device( context, libusb_log );
	check( transfer->handle != NULL, "Unable to open INL retro-prog usb device handle.");
	
	//TEST flag for development use to provide means to only call test.c functions
	if (T_flag) {
		test_function( transfer );
		goto close;
	}

	//create board object/struct 
	cartridge *cart = malloc( sizeof(cartridge));	
	check_mem(cart);

	//set all cart elements to UNKNOWN
	init_cart_elements(cart);

	// -x flag turns off all autodection
	if (!x_flag) {
		//attempt to detect board inserted in device
		check(!detect_console( cart, transfer ), "Problem detecting cartridge.");	

		//detect mapper as much as possible
		//TODO first step is mirroring for NES boards
		check(!detect_mirroring( cart, transfer ), "Problem detecting cart mirroring.");	

		//TODO first step for SNES is mapping mode
	
		//By this point we know a lot about the cartridge but for things like NES discrete
		//mappers we'll have to play around with the memory to determine exact mapper
		//detect board manufacturer/flash memories as much as possible

		//detect rom sizes as much as possible

	} else {
		printf("auto-detection turned off\n");
	}

	//read in user files/args that glean info about expected board

	//for now just assume user file/args are correct
	if ( p_value != NULL ) {
		//program file provided at commandline
		check( !open_file( &rom->fileptr, p_value ), "Problem opening file %s", p_value);
		detect_file( rom );
	}

	//compare detections to user args and get permission to continue if there are discrepencies

	//if flashing, determine if erasures are necessary and where

	//erase required sectors of flash

	//forced to erase board regardless of current status
	if (e_flag || p_value) {
		erase_nes( transfer );
	}

	//if flashing determine auto-doubling for oversized flash

	//determine if rom can be flashed in a manner to make board compatible with rom
	//ie CNROM/colordreams can be over flashed to play NROM
	//BNROM can be overflashed to simulate UNROM
	//SUROM can be overflashed to run as SNROM

	//determine if snes input rom needs deinterleaved

	//dump or flash data based on user args

	//find some fun trivia to present to user while waiting for flash operatoin..?

	//perform CRC checking to check integrity of dump/flash operation

	//handle simple LED ON/OFF within main for now
	//TODO cut this newbie code out of here
	int xfr_cnt = 0;
	uint8_t rbuf[8];

	int i;

	if (o_flag | n_flag) {
		printf("before return buffer: ");
		for (i = 0; i < 8; i++) {
			rbuf[i] = 7;
			printf("%x ", rbuf[i]);
		}
		printf("\n");
		transfer->endpoint = USB_IN;
		transfer->request = DICT_PINPORT;
		if (o_flag) transfer->wValueLSB = LED_ON;
		if (n_flag) transfer->wValueLSB = LED_OFF;
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


close:
	close_usb( context, transfer->handle);	

	if(rom->fileptr != NULL){
	//close file
		fclose(rom->fileptr);
	}
	
	return 0;

error:
//	printf("main program went to error\n");
//
//	close_usb( context, transfer->handle);	
//	if(rom->fileptr != NULL){
//		fclose(rom->fileptr);
//	}

	return 1;

}
