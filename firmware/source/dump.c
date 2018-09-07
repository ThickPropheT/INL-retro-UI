#include "dump.h"

/* Desc:Dump cart memory into buffer's data array
 * Pre: buffer elements must be updated to designate how/where to dump
 * 	buffer's cur_byte must be cleared or set to where to start dumping
 * Post:page dumped from cart memory to buffer.
 * Rtn: SUCCESS or ERROR# depending on if there were errors.
 */
uint8_t dump_buff( buffer *buff ) {

	uint8_t addrH = buff->page_num;	//A15:8  while accessing page
	uint8_t	bank;
//warn	uint8_t addrX;	//A23:16 while accessing page

	//TODO use mapper to set mapper controlled address bits

	//use mem_type to set addrH/X as needed for dump loop
	//also use to get read function pointer
	switch ( buff->mem_type ) {
		case PRGROM:
			addrH |= 0x80;	//$8000
			if (buff->mapper == MMC1) {
				//write bank value to bank table
				//page_num shift by 6 bits A15 >> A9(1)
				bank = (buff->page_num)>>6;
				bank &= 0x0F;		//only 4 bits in PRG
				//LSb doesn't matter in 32KB mode
				mmc1_wr(0x8000, 0x10, 1);	//write bank to PRG-ROM bank register
				mmc1_wr(0xE000, bank, 0);	//write bank to PRG-ROM bank register
				//TODO SXROM/SUROM require writting PRG-ROM MSb of address to CHR registers
			}
			if (buff->mapper == UxROM) {
				//addrH &= 0b1011 1111 A14 must always be low
				addrH &= 0xBF;
				//write bank value to bank table
				//page_num shift by 6 bits A14 >> A8(0)
				bank = (buff->page_num)>>6;
				//Nomolos bank table @ CC84
				//nes_cpu_wr( (0xCC84+bank), bank );
				//Owlia bank table @ CC84
				//nes_cpu_wr( (0xE473+bank), bank );
				//Rushnattack
				//nes_cpu_wr( (0x8000+bank), bank );
				//twindragons
				//nes_cpu_wr( (0xC000+bank), bank );
				//h1
				//nes_cpu_wr( (0xFFC0+bank), bank );
				//AFB
				nes_cpu_wr( (0xFD69+bank), bank );

				buff->cur_byte = nes_cpu_page_rd_poll( buff->data, addrH, buff->id, 
								//id contains MSb of page when <256B buffer
								buff->last_idx, ~FALSE );
				break;
			}
			if (buff->mapper == MAP30) {
				//addrH &= 0b1011 1111 A14 must always be low
				addrH &= 0xBF;
				//write bank value to bank table
				//page_num shift by 6 bits A14 >> A8(0)
				bank = (buff->page_num)>>6;
				//mapper register $C000-FFFF
				nes_cpu_wr( 0xC000, bank );

				buff->cur_byte = nes_cpu_page_rd_poll( buff->data, addrH, buff->id, 
								//id contains MSb of page when <256B buffer
								buff->last_idx, ~FALSE );
				break;
			}
			if ((buff->mapper == BxROM) || (buff->mapper == CDREAM)) {
				//write bank value to bank table
				//page_num shift by 7 bits A15 >> A8(0)
				bank = (buff->page_num)>>7;
				//Lizard bank table @ FF94
				nes_cpu_wr( (0xFF94+bank), bank );
				//HH85
				//nes_cpu_wr( (0xFFE0+bank), bank );
				//Mojon bank table @ FF94
				//nes_cpu_wr( 0x800C, 0x00);	//select first bank (only one with table)
				//nes_cpu_wr( (0xCC43+bank), bank );	//then select desired bank
			}
			if (buff->mapper == A53) {
				//write bank value to bank table
				//page_num shift by 7 bits A15 >> A8(0)
				bank = (buff->page_num)>>7;
				//Setup as CNROM, then scroll through outer banks.
				//cpu_wr(0x5000, 0x80); //reg select mode
				//   xxSSPPMM   SS-size: 0-32KB, PP-prg mode: 0,1 32KB, MM-mirror
				//cpu_wr(0x8000, 0b00000000);   //reg value 256KB inner, 32KB banks
				nes_cpu_wr(0x5000, 0x81); //outer reg select mode
				nes_cpu_wr(0x8000, bank);	  //outer bank
				nes_cpu_wr(0x5000, 0x00); //chr reg select act like CNROM
			}
			if (buff->mapper == EZNSF) {
				//addrH &= 0b1000 1111 A14-12 must always be low
				addrH &= 0x8F;
				//write bank value to bank table
				//page_num shift by 4 bits A12 >> A8(0)
				bank = (buff->page_num)>>4;
				nes_cpu_wr(0x5000, bank);	  //bank @ $8000-8FFF
			}
			buff->cur_byte = nes_cpu_page_rd_poll( buff->data, addrH, buff->id, 
								//id contains MSb of page when <256B buffer
								buff->last_idx, ~FALSE );
			break;

		case CHRROM:		//$0000
			if (buff->mapper == NROM) {
				buff->cur_byte = nes_ppu_page_rd_poll( buff->data, addrH, buff->id,
									buff->last_idx, ~FALSE );
			}

			if (buff->mapper == CDREAM) {
				//select bank
				//8KB banks $0000-1FFF
				//page_num shift by 5 bits A13 >> A8(0)
				bank = (buff->page_num)>>5;

				//write bank to register
				//TODO account for bus conflicts
				nes_cpu_wr(0xFFFF, bank<<4);
				
				addrH &= 0x1F;	//only A12-8 are directly addressable
				buff->cur_byte = nes_ppu_page_rd_poll( buff->data, addrH, buff->id,
									buff->last_idx, ~FALSE );
			}

			if (buff->mapper == DPROM) {
				//select bank
				//8KB banks $0000-1FFF
				//page_num shift by 5 bits A13 >> A8(0)
				bank = (buff->page_num)>>5;

				//write bank to register
				nes_ppu_wr(0x3FFF, bank);
				
				addrH &= 0x1F;	//only A12-8 are directly addressable
				buff->cur_byte = nes_dualport_page_rd_poll( buff->data, addrH, buff->id,
									buff->last_idx, ~FALSE );
			}

			if (buff->mapper == MMC1) {
				//write bank value to bank table
				//page_num shift by 4 bits A12 >> A8(0)
				bank = (buff->page_num)>>4;
				bank &= 0x1F;		//only 5 bits in CHR regs
				//LSb doesn't matter in 32KB mode
				mmc1_wr(0x8000, 0x10, 1);	//set to 4KB bank mode
				mmc1_wr(0xA000, bank, 0);	//write bank to CHR-ROM bank register
				//TODO SXROM/SUROM require writting PRG-ROM MSb of address to CHR registers
				
				addrH &= 0x0F;	//only A11-8 are directly addressable
				buff->cur_byte = nes_ppu_page_rd_poll( buff->data, addrH, buff->id,
									buff->last_idx, ~FALSE );
			}

			break;
		case PRGRAM:
			addrH |= 0x60;	//$6000
			buff->cur_byte = nes_cpu_page_rd_poll( buff->data, addrH, buff->id,
								buff->last_idx, ~FALSE );
			break;
		case SNESROM:
			if (buff->mapper == LOROM) {
				addrH |= 0x80;	//$8000 LOROM space
				//need to split page_num
				//A14-8 page_num[6-0]
				//A15 high (LOROM)
				//A23-16 page_num[14-7]
				bank = (buff->page_num)>>7;
			}
			if (buff->mapper == HIROM) {
				//need to split page_num
				//A15-8 page_num[7-0]
				//A21-16 page_num[13-8]
				//A22 high (HIROM)
				//A23 ~page_num[14] (bank CO starts first half, bank 40 starts second)
				bank = ((((buff->page_num)>>8) | 0x40) & 0x7F);
			}
			HADDR_SET( bank );
			buff->cur_byte = snes_rom_page_rd_poll( buff->data, addrH, buff->id, 
									//id contains MSb of page when <256B buffer
									buff->last_idx, ~FALSE );
		case SNESRAM:
//warn			addrX = ((buff->page_num)>>8);
			break;
		default:
			return ERR_BUFF_UNSUP_MEM_TYPE;
	}

	return SUCCESS;
}
