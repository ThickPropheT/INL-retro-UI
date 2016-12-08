#include "nes.h"

/* Desc:check if PPU /A13 -> CIRAM /CE jumper present
 *	Does NOT check if PPU A13 is inverted and then drives CIRAM /CE
 * Pre: nes_init() been called to setup i/o
 * Post:PPU /A13 left high (disabled), all other ADDRH signals low
 * Rtn: FALSE if jumper is not set
 */
int jumper_ciramce_ppuA13n( USBtransfer *transfer ) 
{
	uint8_t rv[RV_DATA0_IDX+1];

	//check that we can clear CIRAM /CE with PPU /A13
	dictionary_call( transfer, DICT_PINPORT, 	ADDRH_SET,	0,		0,	
					USB_IN,		NULL,	1);
	//read CIRAM /CE pin
	dictionary_call( transfer, DICT_PINPORT, 	CICE_RD,	0,		0,	
					USB_IN,		rv,	RV_DATA0_IDX+1);

	//CIRAM /CE's port PIN register contents are now in rv[RV_DATA_IDX]
	//need to mask out the CIRAM /CE pin
	if ( rv[RV_DATA0_IDX] & CICE_MSK ) {
		//CIRAM /CE pin was always high regardless of PPU /A13
		debug("CIRAM /CE high when /A13 low ");
		return FALSE;
	}

	//set PPU /A13 high
	dictionary_call( transfer, DICT_PINPORT, 	ADDRH_SET,	PPU_A13N_MSK,	0,	
					USB_IN,		NULL,	1);
	//read CIRAM /CE pin
	dictionary_call( transfer, DICT_PINPORT, 	CICE_RD,	0,		0,	
					USB_IN,		rv,	RV_DATA0_IDX+1);

	//CIRAM /CE's port PIN register contents are now in rv[RV_DATA_IDX]
	//need to mask out the CIRAM /CE pin
	if ( (rv[RV_DATA0_IDX] & CICE_MSK) == 0 ) {
		//CICE jumper not present
		debug("CIRAM /CE low when /A13 high ");
		return FALSE;
	}

	//CICE low jumper appears to be present
	debug("CIRAM /CE <- PPU /A13 jumper present");
	return ~FALSE;

}

/* Desc:check if PPU A13 is inverted then drives CIRAM /CE 
 *	Some mappers may do this including INLXO-ROM boards
 *	Does NOT check if PPU /A13 is drives CIRAM /CE
 * Pre: nes_init() been called to setup i/o
 * Post:PPU A13 left disabled (hi)
 * Rtn: FALSE if inverted PPU A13 doesn't drive CIRAM /CE
 */
int ciramce_inv_ppuA13( USBtransfer *transfer ) 
{
	uint8_t rv[RV_DATA0_IDX+1];

	//set PPU /A13 low
	dictionary_call( transfer, DICT_PINPORT, 	ADDRH_SET,	0,		0,	
					USB_IN,		NULL,	1);
	//read CIRAM /CE pin
	dictionary_call( transfer, DICT_PINPORT, 	CICE_RD,	0,		0,	
					USB_IN,		rv,	RV_DATA0_IDX+1);

	// CIRAM /CE should be high if inverted A13 is what drives it
	if ( (rv[RV_DATA0_IDX] & CICE_MSK) == 0 ) {
		//CICE jumper not present
		debug("CIRAM /CE low when /A13 low ");
		return FALSE;
	}

	//check that we can clear CIRAM /CE with PPU /A13 high
	dictionary_call( transfer, DICT_PINPORT, 	ADDRH_SET,	PPU_A13_MSK,	0,	
					USB_IN,		NULL,	1);
	//read CIRAM /CE pin
	dictionary_call( transfer, DICT_PINPORT, 	CICE_RD,	0,		0,	
					USB_IN,		rv,	RV_DATA0_IDX+1);

	// CIRAM /CE should be low if inverted A13 is what drives it
	if ( rv[RV_DATA0_IDX] & CICE_MSK ) {
		//CIRAM /CE pin was always high regardless of PPU /A13
		debug("CIRAM /CE high when /A13 high ");
		return FALSE;
	}


	//CICE low jumper appears to be present
	debug("CIRAM /CE <- inverse PPU /A13");
	return ~FALSE;

}


/* Desc:check for famicom audio in->out jumper
 *	This drives EXP6 (RF out) -> EXP0 (APU in) which is backwards..
 *	not much can do about that for current kazzo designs
 *	There are probably caps/resistors for synth carts anyway
 *	but to be safe only apply short pulses.
 *	While we typically don't want to apply 5v to EXP port on NES carts,
 *	this only does so for EXP6 which is safe on current designs.
 *	All other EXP1-8 pins are only driven low.
 * Pre: nes_init() been called to setup i/o
 *	which makes EXP0 floating i/p
 * Post:EXP FF left disabled and EXP0 floating
 *	AXLOE pin returned to input with pullup
 * Rtn: FALSE if jumper/connection is not present
 * Test:Works on non-expansion sound carts obviously
 *	Works on VRC6 and VRC7
 *	Others untested
 */
int famicom_sound( USBtransfer *transfer ) 
{
	uint8_t rv[RV_DATA0_IDX+1];

	//EXP0 should be floating input
	//AXLOE pin needs to be set as output and
	//EXP FF needs enabled before we can clock it, 
	//but don't leave it enabled before exiting function

	//set AXLOE to output
	dictionary_call( transfer, DICT_PINPORT, 	AXLOE_OP,	0,		0,	
					USB_IN,		NULL,	1);
	//enable EXP FF
	dictionary_call( transfer, DICT_PINPORT, 	EXPFF_OP,	0,		0,	
					USB_IN,		NULL,	1);
	//Latch low first
	dictionary_call( transfer, DICT_PINPORT, 	ADDRX_SET,	0,		0,	
					USB_IN,		NULL,	1);
	//read EXP0 Famicom APU audio pin
	dictionary_call( transfer, DICT_PINPORT, 	FC_APU_RD,	0,		0,	
					USB_IN,		rv,	RV_DATA0_IDX+1);

	//need to mask out the pin
	if ( rv[RV_DATA0_IDX] & FC_APU_MSK ) {
		debug("RF audio out (EXP6) didn't drive APU audio in (EXP0) low");
		//disable EXP FF
		dictionary_call( transfer, DICT_PINPORT, 	EXPFF_FLT,	0,		0,	
						USB_IN,		NULL,	1);
		//retun AXLOE to input
		dictionary_call( transfer, DICT_PINPORT, 	AXLOE_IP,	0,		0,	
					USB_IN,		NULL,	1);
		return FALSE;
	}

	//Latch pin high 
	dictionary_call( transfer, DICT_PINPORT, 	ADDRX_SET,	FC_RF_MSK,	0,	
					USB_IN,		NULL,	1);
	//read EXP0 Famicom APU audio pin
	dictionary_call( transfer, DICT_PINPORT, 	FC_APU_RD,	0,		0,	
					USB_IN,		rv,	RV_DATA0_IDX+1);
	//disable EXP FF
	dictionary_call( transfer, DICT_PINPORT, 	EXPFF_FLT,	0,		0,	
					USB_IN,		NULL,	1);
	//retun AXLOE to input
	dictionary_call( transfer, DICT_PINPORT, 	AXLOE_IP,	0,		0,	
					USB_IN,		NULL,	1);

	//mask pin from byte
	if ( (rv[RV_DATA0_IDX] & FC_APU_MSK) == 0 ) {
		debug("RF audio out (EXP6) didn't drive APU audio in (EXP0) high");
		return FALSE;
	}

	//CICE low jumper appears to be present
	debug("RF audio out (EXP6) is connected to APU audio in (EXP0)");
	return ~FALSE;

}
