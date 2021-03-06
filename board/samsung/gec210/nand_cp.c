﻿/*
 * $Id: nand_cp.c,v 1.1 2008/11/20 01:08:36 boyko Exp $
 *
 * (C) Copyright 2006 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * You must make sure that all functions in this file are designed
 * to load only U-Boot image.
 *
 * So, DO NOT USE in common read.
 *
 * By scsuh.
 */

/*
 * Large block only: Small block is not supported.
 * No ECC operation: Bit error is not corrected.
 */

#include <common.h>

#if 1 //def CONFIG_S5PC11X
//#include <asm/io.h>
#include <linux/mtd/rawnand.h>
//#include <regs.h>
#include <asm/arch/s5pc110.h>

#define NAND_DISABLE_CE()	(NFCONT_REG |= (1 << 1))
#define NAND_ENABLE_CE()	(NFCONT_REG &= ~(1 << 1))
#define NF_TRANSRnB()		do { while(!(NFSTAT_REG & (1 << 0))); } while(0)

#define NAND_CONTROL_ENABLE()	(NFCONT_REG |= (1 << 0))


static void nand_readpage (ulong col, ulong row, uchar* buf, int len)
{
	int i;

	NAND_ENABLE_CE();

	NFCMD_REG = NAND_CMD_READ0;

	/* Write Column Address */
	NFADDR_REG = (col) & 0xff;
	NFADDR_REG = (col >> 8) & 0xff;

	/* Write Row Address */
	NFADDR_REG = (row) & 0xff;
	NFADDR_REG = (row >> 8) & 0xff;
	NFADDR_REG = (row >> 16) & 0xff;

	NFCMD_REG = NAND_CMD_READSTART;

	NF_TRANSRnB();

	for (i = 0; i < len; i++) {
		buf[i] = NFDATA8_REG;
	}

	NAND_DISABLE_CE();
}

static int nand_isbad (ulong addr)
{
	int page_size = 2048;
	uchar oob[2];

	if (addr == 0)
		return 0;

	nand_readpage(page_size, addr, oob, 2);

	if ((oob[0] == 0xFF) && (oob[1] == 0xFF))
		return 0;
	else
		return 1;
}

/*
 * address format
 *              17 16         9 8            0
 * --------------------------------------------
 * | block(12bit) | page(5bit) | offset(9bit) |
 * --------------------------------------------
 */

static int nandll_read_page (uchar *buf, ulong addr)
{
	int page_size = 2048;

	nand_readpage(0, addr, buf, page_size);

        return 0;
}

/*
 * Read data from NAND.
 */
static int nandll_read_blocks (ulong dst_addr, ulong size)
{
        uchar *buf = (uchar *)dst_addr;
        int i;

#if defined(CONFIG_SECURE_BOOT)
	int skipped_page = 6;
#else
	int skipped_page = 0;
#endif

	uint page_shift = 11;

        /* Read pages */
        for (i = 0; i < (size>>page_shift); i++, buf+=(1<<page_shift)) {
		if ((i & (64 - 1)) == 0) {
			while (nand_isbad(i + skipped_page)) {
				skipped_page += 64;
			}
		}
                nandll_read_page(buf, i + skipped_page);
        }
        return 0;
}

int copy_uboot_to_ram (void)
{
	int i;
	vu_char id;

	int rv;

	NAND_CONTROL_ENABLE();
        NAND_ENABLE_CE();
        NFCMD_REG = NAND_CMD_READID;
        NFADDR_REG = 0x00;

	/* wait for a while */
        for (i=0; i<200; i++);
	id = NFDATA8_REG;
	id = NFDATA8_REG;

	if (id <= 0x80)
		return -1;	// Do not support small page (512B) any more

	/* read NAND blocks */
	rv = nandll_read_blocks(CONFIG_SYS_TEXT_BASE, COPY_BL2_SIZE);

#if defined(CONFIG_SECURE_BOOT)
	rv = Check_Signature((SecureBoot_CTX *)SECURE_BOOT_CONTEXT_ADDR,
			(unsigned char *)CFG_PHY_UBOOT_BASE,
			(1024*512 - 128),
			(unsigned char *)(CFG_PHY_UBOOT_BASE + (1024*512-128)),
			128);
	if (rv != SB_OK)
		while(1);
#endif

	return rv;
}
 
#endif

