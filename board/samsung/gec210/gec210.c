﻿/*
 *  Copyright (C) 2008-2009 Samsung Electronics
 *  Minkyu Kang <mk7.kang@samsung.com>
 *  Kyungmin Park <kyungmin.park@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/sromc.h>
#include <netdev.h>
#include <asm/mach-types.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * Miscellaneous platform dependent initialisations
 */
#ifdef CONFIG_SMC911X
static void smc9115_pre_init(void)
{
	u32 smc_bw_conf, smc_bc_conf;

	/* gpio configuration GPK0CON */
	gpio_cfg_pin(S5PC100_GPIO_K00 + CONFIG_ENV_SROM_BANK, S5P_GPIO_FUNC(2));

	/* Ethernet needs bus width of 16 bits */
	smc_bw_conf = SMC_DATA16_WIDTH(CONFIG_ENV_SROM_BANK);
	smc_bc_conf = SMC_BC_TACS(0x0) | SMC_BC_TCOS(0x4) | SMC_BC_TACC(0xe)
			| SMC_BC_TCOH(0x1) | SMC_BC_TAH(0x4)
			| SMC_BC_TACP(0x6) | SMC_BC_PMC(0x0);

	/* Select and configure the SROMC bank */
	s5p_config_sromc(CONFIG_ENV_SROM_BANK, smc_bw_conf, smc_bc_conf);
}
#endif

#ifdef CONFIG_DRIVER_DM9000
static void dm9000_pre_init(void)
{
	unsigned int tmp;

	/* DM9000 on SROM BANK1, 16 bit */
	SROM_BW_REG &= ~(0xf << 4);
	SROM_BW_REG |= (0x1 << 4);
	SROM_BC1_REG = ((0<<28)|(0<<24)|(5<<16)|(0<<12)|(0<<8)|(0<<4)|(0<<0));
	/* Set MP01_1 as SROM_CSn[1] */
	tmp = MP01CON_REG;
	tmp &=~(0xf<<4);
	tmp |=(2<<4);
	MP01CON_REG = tmp;
}
#endif

int board_init(void)
{
#ifdef CONFIG_SMC911X
	smc9115_pre_init();
#endif

#ifdef CONFIG_DRIVER_DM9000
	dm9000_pre_init();
#endif
	/* bi_arch_number has set in setup_machine(). */
	/* gd->bd->bi_arch_number = MACH_TYPE_SMDKC110; */
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	gd->ram_size += get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE);

	return 0;
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = PHYS_SDRAM_2_SIZE;

	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	printf("Board:\tGEC210\n");
	return 0;
}
#endif

int board_eth_init(bd_t *bis)
{
	int rc = 0;

#ifdef CONFIG_SMC911X
	rc = smc911x_initialize(0, CONFIG_SMC911X_BASE);
#endif

#ifdef CONFIG_DRIVER_DM9000
	rc = dm9000_initialize(bis);
#endif
	return rc;
}
