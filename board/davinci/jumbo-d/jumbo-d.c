/*
 * Copyright (C) 2012 Bticino S.p.A.
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
	gd->bd->bi_arch_number = MACH_TYPE_JUMBO_D;
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	icache_enable();
	invalidate_dcache_all();
	flush_dcache_all();
	dcache_enable();
	printf("MACH_TYPE == %4d [JUMBO-D]\n", gd->bd->bi_arch_number);

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

#ifdef CONFIG_DAVINCI_MMC

static struct davinci_mmc mmc_sd0 = {
	.reg_base = (struct davinci_mmc_regs *)DAVINCI_MMC_SD0_BASE,
	.input_clk =  86000000,
	.voltages = MMC_VDD_32_33 | MMC_VDD_33_34,
	.version = MMC_CTLR_VERSION_2,
	.host_caps = MMC_MODE_4BIT | MMC_MODE_HS | MMC_MODE_HS_52MHz,
};

#ifdef CONFIG_DAVINCI_MMC_SD1 /* TODO do not work */
/*
	NOTA: is unusefull because in drivers/mmc/mmc.c was been modify
		drivers/mmc/mmc.c to do not test SD card
*/
static struct davinci_mmc mmc_sd1 = {
	.reg_base = (struct davinci_mmc_regs *)DAVINCI_MMC_SD1_BASE,
	.input_clk = 121500000,
	.host_caps = MMC_MODE_4BIT,
	.voltages =  MMC_VDD_32_33 | MMC_VDD_33_34,
	.version =  MMC_CTLR_VERSION_2,
};
#endif

int board_mmc_init(bd_t *bis)
{
	int err;

	struct davinci_gpio *gpio23_base =
			(struct davinci_gpio *)DAVINCI_GPIO_BANK23;

	/* GIO59 (~eMMC_RESET) pinmux setting */
	writel((readl(PINMUX2) & 0xFFFFFFBF), PINMUX2);

	/* set GIO59 (~eMMC_RESET) output */
	writel((readl(&gpio23_base->dir) & ~(1 << 27)), &gpio23_base->dir);

	/* GIO59 (~eMMC_RESET) output High */
	writel((readl(&gpio23_base->set_data) | (1 << 27)),
						&gpio23_base->set_data);

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
