/*
 * Copyright (C) 2009 Texas Instruments Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/emif_defs.h>
#include <asm/arch/nand_defs.h>
#include <net.h>
#include <netdev.h>
#ifdef CONFIG_DAVINCI_MMC
#include <mmc.h>
#include <asm/arch/sdmmc_defs.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	gd->bd->bi_arch_number = MACH_TYPE_OWL;
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	icache_enable();
	invalidate_dcache_all();
	flush_dcache_all();
	dcache_enable();
	return 0;
}

void enable_vbus(void)
{}
#ifdef CONFIG_DRIVER_TI_EMAC
int board_eth_init(bd_t *bis)
{
	unsigned char enetaddr[6];

	/* Configure PINMUX 3 to enable EMAC pins */
	writel((readl(PINMUX3) | 0x1affff), PINMUX3);

	/* Using this function for setting a random mac address */
	eth_random_enetaddr(enetaddr);
	eth_setenv_enetaddr("ethaddr", enetaddr);

	davinci_emac_initialize();

	return 0;
}
#endif

#ifdef CONFIG_NAND_DAVINCI
static void nand_davinci_select_chip(struct mtd_info *mtd, int chip)
{
	struct nand_chip	*this = mtd->priv;
	unsigned long		wbase = (unsigned long) this->IO_ADDR_W;
	unsigned long		rbase = (unsigned long) this->IO_ADDR_R;

	if (chip == 1) {
		__set_bit(14, &wbase);
		__set_bit(14, &rbase);
	} else {
		__clear_bit(14, &wbase);
		__clear_bit(14, &rbase);
	}
	this->IO_ADDR_W = (void *)wbase;
	this->IO_ADDR_R = (void *)rbase;
}

int board_nand_init(struct nand_chip *nand)
{
	davinci_nand_init(nand);
	nand->options |= NAND_SKIP_BBTSCAN;
	nand->select_chip = nand_davinci_select_chip;
	return 0;
}
#endif

#ifdef CONFIG_DAVINCI_MMC

static struct davinci_mmc mmc_sd0 = {
	.reg_base = (struct davinci_mmc_regs *)DAVINCI_MMC_SD0_BASE,
	.input_clk = 121500000,
	.host_caps = MMC_MODE_4BIT,
	.voltages = MMC_VDD_32_33 | MMC_VDD_33_34,
	.version = MMC_CTLR_VERSION_2,
};

#ifdef CONFIG_DAVINCI_MMC_SD1
static struct davinci_mmc mmc_sd1 = {
	.reg_base = (struct davinci_mmc_regs *)DAVINCI_MMC_SD1_BASE,
	.input_clk = 121500000,
	.host_caps = MMC_MODE_4BIT,
	.voltages = MMC_VDD_32_33 | MMC_VDD_33_34,
	.version = MMC_CTLR_VERSION_2,
};
#endif

int board_mmc_init(bd_t *bis)
{
	int err;

	/* Add slot-0 to mmc subsystem */
	err = davinci_mmc_init(bis, &mmc_sd0);
	if (err)
		return err;

#ifdef CONFIG_DAVINCI_MMC_SD1
#define PUPDCTL1		0x01c4007c
	/* PINMUX(4)-DAT0-3/CMD;  PINMUX(0)-CLK */
	writel((readl(PINMUX4) | 0x55400000), PINMUX4);
	writel((readl(PINMUX0) | 0x00010000), PINMUX0);

	/* Configure MMC/SD pins as pullup */
	writel((readl(PUPDCTL1) & ~0x07c0), PUPDCTL1);

	/* Add slot-1 to mmc subsystem */
	err = davinci_mmc_init(bis, &mmc_sd1);
#endif

	return err;
}

#endif
