/*
 * (C) Copyright 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * HeungJun Kim <riverful.kim@samsung.com>
 * Inki Dae <inki.dae@samsung.com>
 *
 * Configuation settings for the SAMSUNG SMDKC100 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_SAMSUNG		1	/* in a SAMSUNG core */
#define CONFIG_S5P		1	/* which is in a S5P Family */
#define CONFIG_S5PC110		1	/* which is in a S5PC110 / S5PV210 */
#if 0
#define CONFIG_GEC210		1	/* working with GEC210 */
#endif
#define CONFIG_MACH_TYPE		MACH_TYPE_SMDKV210


#include <asm/arch/cpu.h>		/* get chip and board defs */
#include <asm/arch/s5pc110.h>

#define CONFIG_ARCH_CPU_INIT

/* the SMDK6400 has 24MHz input clock */
#define CONFIG_SYS_CLK_FREQ	24000000


/* DRAM Base */
#define CONFIG_SYS_SDRAM_BASE		0x30000000

/* Text Base */
#define CONFIG_SYS_TEXT_BASE		0x34800000

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_EDITING

/*
 * Size of malloc() pool
 * 1MB = 0x100000, 0x100000 = 1024 * 1024
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))

/*
 * select serial console configuration
 */
#define CONFIG_SERIAL0			1	/* use SERIAL 0 on SMDKC100 */

/* PWM */
#define CONFIG_PWM			1

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS

#define CONFIG_BOOTCOMMAND	"run bootk"

#define CONFIG_RAMDISK_BOOT	"root=/dev/ram0 rw rootfstype=ext2" \
				" console=ttySAC0,115200n8" \
				" mem=512M"

#define CONFIG_COMMON_BOOT	"console=ttySAC0,115200n8" \
				" mem=512M " \
				" " CONFIG_MTDPARTS_DEFAULT

#define CONFIG_UPDATEB	"updateb=tftp 0x40000000 u-boot.bin;" \
			"nand erase.part boot;" \
			"nand write 0x40000000 boot\0"

#define CONFIG_ENV_OVERWRITE
#define CONFIG_EXTRA_ENV_SETTINGS					\
	CONFIG_UPDATEB \
	"updatek=" \
		"tftp 0x40000000 uImage;" \
		"nand erase.part kernel;" \
		"nand write 0x40000000 kernel\0" \
	"updateu=" \
		"onenand erase block 147-4095;" \
		"onenand write 0x32000000 0x1260000 0x8C0000\0" \
	"bootk=" \
		"nand read 0x30007FC0 kernel;" \
		"bootm 0x30007FC0\0" \
	"flashboot=" \
		"set bootargs root=/dev/mtdblock${bootblock} " \
		"rootfstype=${rootfstype} " \
		"ubi.mtd=${ubiblock} ${opts} " CONFIG_COMMON_BOOT ";" \
		"run bootk\0" \
	"ubifsboot=" \
		"set bootargs root=ubi0!rootfs rootfstype=ubifs " \
		" ubi.mtd=${ubiblock} ${opts} " CONFIG_COMMON_BOOT "; " \
		"run bootk\0" \
	"boottrace=setenv opts initcall_debug; run bootcmd\0" \
	"android=" \
		"set bootargs root=ubi0!ramdisk ubi.mtd=${ubiblock} " \
		"rootfstype=ubifs init=/init.sh " CONFIG_COMMON_BOOT "; " \
		"run bootk\0" \
	"nfsboot=" \
		"set bootargs root=/dev/nfs ubi.mtd=${ubiblock} " \
		"nfsroot=${nfsroot},nolock " \
		"ip=${ipaddr}:${serverip}:${gatewayip}:" \
		"${netmask}:nowplus:usb0:off " CONFIG_COMMON_BOOT "; " \
		"run bootk\0" \
	"ramboot=" \
		"set bootargs " CONFIG_RAMDISK_BOOT \
		" initrd=0x33000000,8M ramdisk=8192\0" \
	"rootfstype=cramfs\0" \
	"mtdparts=" CONFIG_MTDPARTS_DEFAULT "\0" \
	"meminfo=mem=512M\0" \
	"nfsroot=/nfsroot/arm\0" \
	"bootblock=5\0" \
	"ubiblock=4\0" \
	"ubi=enabled"

#define CONFIG_NETMASK          255.255.255.0
#define CONFIG_IPADDR		192.168.1.48
#define CONFIG_SERVERIP		192.168.1.46
#define CONFIG_GATEWAYIP	192.168.1.1

/*-----------------------------------------------------------------------
 * System clock configuration
 */
#define APLL_MDIV       0x7d
#define APLL_PDIV       0x3
#define APLL_SDIV       0x1

#define APLL_LOCKTIME_VAL	0x2cf

/* Set AFC value */
#define AFC_ON		0x00000000
#define AFC_OFF		0x10000010


#define MPLL_MDIV	0x29b
#define MPLL_PDIV	0xc
#define MPLL_SDIV	0x1

#define EPLL_MDIV	0x60
#define EPLL_PDIV	0x6
#define EPLL_SDIV	0x2

#define VPLL_MDIV	0x6c
#define VPLL_PDIV	0x6
#define VPLL_SDIV	0x3

/* CLK_DIV0 */
#define APLL_RATIO	0
#define A2M_RATIO	4
#define HCLK_MSYS_RATIO	8
#define PCLK_MSYS_RATIO	12
#define HCLK_DSYS_RATIO	16
#define PCLK_DSYS_RATIO 20
#define HCLK_PSYS_RATIO	24
#define PCLK_PSYS_RATIO 28

#define CLK_DIV0_MASK	0x7fffffff

#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)

#define APLL_VAL	set_pll(APLL_MDIV,APLL_PDIV,APLL_SDIV)
#define MPLL_VAL	set_pll(MPLL_MDIV,MPLL_PDIV,MPLL_SDIV)
#define EPLL_VAL	set_pll(EPLL_MDIV,EPLL_PDIV,EPLL_SDIV)
#define VPLL_VAL	set_pll(VPLL_MDIV,VPLL_PDIV,VPLL_SDIV)

#define CLK_DIV0_VAL    ((0<<APLL_RATIO)|(4<<A2M_RATIO)|(4<<HCLK_MSYS_RATIO)|(1<<PCLK_MSYS_RATIO)\
			|(3<<HCLK_DSYS_RATIO)|(1<<PCLK_DSYS_RATIO)|(4<<HCLK_PSYS_RATIO)|(1<<PCLK_PSYS_RATIO))

#define CLK_DIV1_VAL	((1<<16)|(1<<12)|(1<<8)|(1<<4))
#define CLK_DIV2_VAL	(1<<0)


/*-----------------------------------------------------------------------
 * memory configuration
 */
#define MEMORY_BASE_ADDRESS1	0x40000000	//(2016-10-12 20:26:04 zxd modify)
#define DMC0_MEMCONTROL		0x00202400	//(2016-10-12 20:26:04 zxd modify)
#define DMC0_MEMCONFIG_0	0x30F00313	// MemConfig0	256MB config, 8 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 2:Mixed,(2016-10-12 20:19:53 zxd modify)
#define DMC0_MEMCONFIG_1	0x00F00313	// MemConfig1 (2016-10-12 20:19:53 zxd modify)
#define DMC0_TIMINGA_REF	0x00000618	// TimingAref	7.8us*133MHz=1038(0x40E), 100MHz=780(0x30C), 20MHz=156(0x9C), 10MHz=78(0x4E)
#define DMC0_TIMING_ROW		0x28233287	// TimingRow	for @200MHz
#define DMC0_TIMING_DATA	0x23240304	// TimingData	CL=3
#define	DMC0_TIMING_PWR		0x09C80232	// TimingPower
	
#define	DMC1_MEMCONTROL		0x00202400	// MemControl	BL=4, 2 chip, DDR2 type, dynamic self refresh, force precharge, dynamic power down off
#define DMC1_MEMCONFIG_0	0x40F00313	// MemConfig0	512MB config, 8 banks,Mapping Method[12:15]0:linear, 1:linterleaved, 2:Mixed
#define DMC1_MEMCONFIG_1	0x00F00313	// MemConfig1 (2016-10-12 20:19:53 zxd modify)
#define DMC1_TIMINGA_REF	0x00000618	// TimingAref	7.8us*133MHz=1038(0x40E), 100MHz=780(0x30C), 20MHz=156(0x9C), 10MHz=78(0x4
#define DMC1_TIMING_ROW		0x28233289	// TimingRow	for @200MHz
#define DMC1_TIMING_DATA	0x23240304	// TimingData	CL=3
#define	DMC1_TIMING_PWR		0x08280232	// TimingPower


#define CONFIG_NR_DRAM_BANKS    2          	/* we have 2 bank of DRAM */
#define SDRAM_BANK_SIZE         0x10000000	/* 256MB */
#define PHYS_SDRAM_1            CONFIG_SYS_SDRAM_BASE /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE       SDRAM_BANK_SIZE
#define PHYS_SDRAM_2            (MEMORY_BASE_ADDRESS1) /* SDRAM Bank #2 */
#define PHYS_SDRAM_2_SIZE       SDRAM_BANK_SIZE


/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_PBSIZE	384	/* Print Buffer Size */
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5e00000)
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_MONITOR_BASE	0x00000000

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0xb0e00010
#define CFG_NAND_PAGES_IN_BLOCK		64

#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* 256 KiB */

#if !defined(CONFIG_NAND_SPL) && (CONFIG_SYS_TEXT_BASE >= 0xc0000000)
#define CONFIG_ENABLE_MMU
#endif

#ifdef CONFIG_ENABLE_MMU
#define CONFIG_SYS_MAPPED_RAM_BASE	0xc0000000
#else
#define CONFIG_SYS_MAPPED_RAM_BASE	CONFIG_SYS_SDRAM_BASE
#endif

/*-----------------------------------------------------------------------
 * Boot configuration
 */
#define BOOT_ONENAND		0x1
#define BOOT_NAND		0x2
#define BOOT_MMCSD		0x3
#define BOOT_NOR		0x4
#define BOOT_SEC_DEV		0x5

#define CONFIG_ENV_SIZE			(128 << 10)	/* 128KiB, 0x20000 */
#define CONFIG_ENV_ADDR			(512 << 10)	/* 512KiB, 0x80000 */
#define CONFIG_ENV_OFFSET		(512 << 10)	/* 512KiB, 0x80000 */

/* nand copy size from nand to DRAM.*/
#define	COPY_BL1_SIZE		(8 << 10)	/* for irom's BL0 copy */
#define	COPY_BL2_SIZE		0x80000

#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_LOAD_ADDR - 0x1000000)

/*
 * Ethernet Contoller driver
 */
#define CONFIG_DRIVER_DM9000		1
#define CONFIG_DM9000_BASE		(0x88000000)
#define DM9000_IO			(CONFIG_DM9000_BASE)
#define DM9000_DATA			(CONFIG_DM9000_BASE+8)

#ifdef CONFIG_CMD_NET
#define CONFIG_ENV_SROM_BANK   1       /* Select SROM Bank-1 for Ethernet*/
#endif /* CONFIG_CMD_NET */

#define CONFIG_LCD_LOGO			1
#define LCD_BPP				LCD_COLOR32


#endif	/* __CONFIG_H */
