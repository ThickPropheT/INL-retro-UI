#include "erase.h"

int erase_nes( USBtransfer *transfer ) 
{
	
	debug("erasing");

	int xfr_cnt;
	uint8_t rbuf[2];
	rbuf[0] = 0;
	rbuf[1] = 0;
	int i;

	transfer->endpoint = USB_IN;
        transfer->request = PINPORT;
	transfer->data = rbuf;

	uint8_t c[20];
	uint8_t o[20];

	c[0]  = LED_ON;	
	c[1]  = ADDR_OP;	
	c[2]  = DATA_IP;	
	c[3]  = M2_OP;	
	c[4]  = ROMSEL_OP;	
	c[5]  = PRGRW_OP;	
	c[6]  = CSRD_OP;	
	c[7]  = CSWR_OP;	
	c[8]  = AHL_OP;	
	c[9]  = AXLOE_OP;	
	c[10] = AXL_CLK;	
	c[11] = ADDR_RD;	
	c[12] = ADDR_LO;	
	c[13] = ADDR_RD;	
	c[14] = LED_OFF;	
	c[15] = LED_OFF;	
	c[16] = LED_OFF;	
	c[17] = LED_OFF;	
	c[18] = LED_OFF;	
	c[19] = LED_OFF;	

	for ( i = 0; i < 20; i++) {
		transfer->wValueLSB = c[i];
		if (c[i] >= 0xc0) transfer->wLength = 2;
		else transfer->wLength = 1;
		xfr_cnt = usb_transfer( transfer );
		debug("xf: %d   OP: %d/%x    er: %d rv: %x",xfr_cnt, c[i], c[i], rbuf[0], rbuf[1]);
		rbuf[0] = 0xAA;
		rbuf[1] = 0;
	
	//send command
	}


	return 0;

}
