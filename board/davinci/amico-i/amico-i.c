/*
 * Copyright (C) 2013 Bticino S.p.A.
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
	gd->bd->bi_arch_number = MACH_TYPE_AMICO_I;
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	icache_enable();
	invalidate_dcache_all();
	flush_dcache_all();
	dcache_enable();
	printf("MACH_TYPE == %4d [AMICO-I]\n", gd->bd->bi_arch_number);

	return 0;
}

#ifdef CONFIG_DRIVER_TI_EMAC
int board_eth_init(bd_t *bis)
{
	uint8_t enetaddr[6];

	/* Configure PINMUX 3 to enable EMAC pins */
	writel((readl(PINMUX3) | 0x1affff), PINMUX3);

	/* External PHY reset cycle */
	writel((readl(GPIO_BANK2_REG_DIR_ADDR) & ~0x1000),
	       GPIO_BANK2_REG_DIR_ADDR);
	writel(0x1000, GPIO_BANK2_REG_CLR_ADDR);
	udelay(10000);
	writel(0x1000, GPIO_BANK2_REG_SET_ADDR);
	udelay(120000);

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

	/* Configure PINMUX 2 to only 7bit EM_BUS */
	writel((readl(PINMUX2) & 0xffffffbf), PINMUX2);

	davinci_nand_init(nand);
	nand->options |= NAND_SKIP_BBTSCAN;
	nand->select_chip = nand_davinci_select_chip;
	return 0;
}
#endif

