/*
 * Copyright (C) 2013 Bticino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* Bticino AMICO board */
#define DAVINCI_GEKKO
#define BOARD_STRING "Gekko"

/* RAM size */
#define PHYS_SDRAM_1_SIZE		(128 << 20)	/* 128 MiB */

/* SD/MMC */
#undef CONFIG_MMC
#define CONFIG_NAND_DAVINCI

#undef CONFIG_USB_DAVINCI
#undef CONFIG_MUSB_HCD         /* include support for usb host */
#define CONFIG_USBD_PRODUCTID          0x1122 /* TODO */
#define CONFIG_USBD_PRODUCT_NAME       "gekko"

#include "bt_medium.h"

#endif /* __CONFIG_H */
