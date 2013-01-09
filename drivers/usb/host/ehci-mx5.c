/*
 * Copyright (c) 2009 Daniel Mack <daniel@caiaq.de>
 * Copyright (C) 2010 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include <common.h>
#include <usb.h>
#include <errno.h>
#include <linux/compiler.h>
#include <usb/ehci-fsl.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/clock.h>
#include <asm/arch/mx5x_pins.h>
#include <asm/arch/iomux.h>

#include "ehci.h"

#define MX5_USBOTHER_REGS_OFFSET 0x800


#define MXC_OTG_OFFSET		0
#define MXC_H1_OFFSET		0x200
#define MXC_H2_OFFSET		0x400

#define MXC_USBCTRL_OFFSET		0
#define MXC_USB_PHY_CTR_FUNC_OFFSET	0x8
#define MXC_USB_PHY_CTR_FUNC2_OFFSET	0xc
#define MXC_USB_CTRL_1_OFFSET		0x10
#define MXC_USBH2CTRL_OFFSET		0x14
#define MXC_USBH3CTRL_OFFSET		0x18

/* USB_CTRL */
#define MXC_OTG_UCTRL_OWIE_BIT	(1 << 27) /* OTG wakeup intr enable */
#define MXC_OTG_UCTRL_OPM_BIT	(1 << 24) /* OTG power mask */
#define MXC_H1_UCTRL_H1UIE_BIT	(1 << 12) /* Host1 ULPI interrupt enable */
#define MXC_H1_UCTRL_H1WIE_BIT	(1 << 11) /* HOST1 wakeup intr enable */
#define MXC_H1_UCTRL_H1PM_BIT	(1 << 8) /* HOST1 power mask */

/* USB_PHY_CTRL_FUNC */
#define MXC_OTG_PHYCTRL_OC_DIS_BIT (1 << 8) /* OTG Disable Overcurrent Event */
#define MXC_H1_OC_DIS_BIT	(1 << 5) /* UH1 Disable Overcurrent Event */

/* USBH2CTRL */
#define MXC_H2_UCTRL_H2UIE_BIT	(1 << 8) /* Host2 ULPI interrupt enable */
#define MXC_H2_UCTRL_H2WIE_BIT	(1 << 7) /* HOST2 wakeup intr enable */
#define MXC_H2_UCTRL_H2PM_BIT	(1 << 4) /* HOST2 power mask */

/* USBH3CTRL */
#define MXC_H3_UCTRL_H3UIE_BIT	(1 << 8) /* Host3 ULPI interrupt enable */
#define MXC_H3_UCTRL_H3WIE_BIT	(1 << 7) /* HOST3 wakeup intr enable */
#define MXC_H3_UCTRL_H3PM_BIT	(1 << 4) /* HOST3 power mask */

/* USB_CTRL_1 */
#define MXC_USB_CTRL_UH1_EXT_CLK_EN		(1 << 25)
#define MXC_USB_CTRL_UH2_EXT_CLK_EN		(1 << 26)
#define MXC_USB_CTRL_UH3_EXT_CLK_EN		(1 << 27)

/* USB pin configuration */
#define USB_PAD_CONFIG	(PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST | \
			PAD_CTL_DRV_HIGH | PAD_CTL_100K_PU | \
			PAD_CTL_HYS_ENABLE | PAD_CTL_PUE_PULL)

#ifdef CONFIG_MX51
/*
 * Configure the MX51 USB H1 IOMUX
 */
void setup_iomux_usb_h1(void)
{
	mxc_request_iomux(MX51_PIN_USBH1_STP, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_STP, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_CLK, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_CLK, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_DIR, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_DIR, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_NXT, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_NXT, USB_PAD_CONFIG);

	mxc_request_iomux(MX51_PIN_USBH1_DATA0, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_DATA0, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_DATA1, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_DATA1, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_DATA2, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_DATA2, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_DATA3, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_DATA3, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_DATA4, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_DATA4, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_DATA5, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_DATA5, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_DATA6, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_DATA6, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_USBH1_DATA7, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_USBH1_DATA7, USB_PAD_CONFIG);
}

/*
 * Configure the MX51 USB H2 IOMUX
 */
void setup_iomux_usb_h2(void)
{
#if 1
#define USBH2_PAD_CONFIG	(PAD_CTL_SRE_FAST | PAD_CTL_DRV_HIGH | PAD_CTL_PUE_PULL | PAD_CTL_PKE_ENABLE | PAD_CTL_HYS_ENABLE | PAD_CTL_100K_PU)
	mxc_request_iomux(MX51_PIN_EIM_A24, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_A24, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_A25, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_A25, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_A26, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_A26, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_A27, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_A27, USBH2_PAD_CONFIG);// | PAD_CTL_ODE_OPENDRAIN_NONE);

	mxc_request_iomux(MX51_PIN_EIM_D16, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D16, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D17, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D17, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D18, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D18, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D19, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D19, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D20, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D20, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D21, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D21, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D22, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D22, USBH2_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D23, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D23, USBH2_PAD_CONFIG);
#else
	mxc_request_iomux(MX51_PIN_EIM_A24, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_A24, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_A25, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_A25, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_A26, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_A26, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_A27, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_A27, USB_PAD_CONFIG);

	mxc_request_iomux(MX51_PIN_EIM_D16, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D16, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D17, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D17, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D18, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D18, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D19, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D19, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D20, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D20, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D21, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D21, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D22, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D22, USB_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_EIM_D23, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_EIM_D23, USB_PAD_CONFIG);
#endif
}
#endif

int mxc_set_usbcontrol(int port, unsigned int flags)
{
	unsigned int v;
	void __iomem *usb_base = (void __iomem *)OTG_BASE_ADDR;
	void __iomem *usbother_base;
	int ret = 0;

	usbother_base = usb_base + MX5_USBOTHER_REGS_OFFSET;

	switch (port) {
	case 0:	/* OTG port */
		if (flags & MXC_EHCI_INTERNAL_PHY) {
			v = __raw_readl(usbother_base +
					MXC_USB_PHY_CTR_FUNC_OFFSET);
			if (flags & MXC_EHCI_POWER_PINS_ENABLED)
				/* OC/USBPWR is not used */
				v |= MXC_OTG_PHYCTRL_OC_DIS_BIT;
			else
				/* OC/USBPWR is used */
				v &= ~MXC_OTG_PHYCTRL_OC_DIS_BIT;
			__raw_writel(v, usbother_base +
					MXC_USB_PHY_CTR_FUNC_OFFSET);

			v = __raw_readl(usbother_base + MXC_USBCTRL_OFFSET);
			if (flags & MXC_EHCI_POWER_PINS_ENABLED)
				v |= MXC_OTG_UCTRL_OPM_BIT;
			else
				v &= ~MXC_OTG_UCTRL_OPM_BIT;
			__raw_writel(v, usbother_base + MXC_USBCTRL_OFFSET);
		}
		break;
	case 1:	/* Host 1 Host ULPI */
#ifdef CONFIG_MX51
		/* The clock for the USBH1 ULPI port will come externally
		   from the PHY. */
		v = __raw_readl(usbother_base + MXC_USB_CTRL_1_OFFSET);
		__raw_writel(v | MXC_USB_CTRL_UH1_EXT_CLK_EN, usbother_base +
				MXC_USB_CTRL_1_OFFSET);
#endif

		v = __raw_readl(usbother_base + MXC_USBCTRL_OFFSET);
		if (flags & MXC_EHCI_POWER_PINS_ENABLED)
			v &= ~MXC_H1_UCTRL_H1PM_BIT; /* HOST1 power mask used */
		else
			v |= MXC_H1_UCTRL_H1PM_BIT; /* HOST1 power mask used */
		__raw_writel(v, usbother_base + MXC_USBCTRL_OFFSET);

		v = __raw_readl(usbother_base + MXC_USB_PHY_CTR_FUNC_OFFSET);
		if (flags & MXC_EHCI_POWER_PINS_ENABLED)
			v &= ~MXC_H1_OC_DIS_BIT; /* OC is used */
		else
			v |= MXC_H1_OC_DIS_BIT; /* OC is not used */
		__raw_writel(v, usbother_base + MXC_USB_PHY_CTR_FUNC_OFFSET);

		break;
	case 2: /* Host 2 ULPI */
#ifdef CONFIG_MX51
		/* The clock for the USBH2 ULPI port will come externally
		   from the PHY. */
		v = __raw_readl(usbother_base + MXC_USB_CTRL_1_OFFSET);
		__raw_writel(v | MXC_USB_CTRL_UH2_EXT_CLK_EN | MXC_USB_CTRL_UH1_EXT_CLK_EN,
		    usbother_base + MXC_USB_CTRL_1_OFFSET);
#endif


		v = __raw_readl(usbother_base + MXC_USBH2CTRL_OFFSET);
		if (flags & MXC_EHCI_POWER_PINS_ENABLED)
			v &= ~MXC_H2_UCTRL_H2PM_BIT; /* HOST2 power mask used */
		else
			v |= MXC_H2_UCTRL_H2PM_BIT; /* HOST2 power mask used */
		__raw_writel(v, usbother_base + MXC_USBH2CTRL_OFFSET);

//		v = __raw_readl(usbother_base + MXC_USB_PHY_CTR_FUNC_OFFSET);
//		if (flags & MXC_EHCI_POWER_PINS_ENABLED)
//			v &= ~MXC_H1_OC_DIS_BIT; /* OC is used */
//		else
//			v |= MXC_H1_OC_DIS_BIT; /* OC is not used */
//		__raw_writel(v, usbother_base + MXC_USB_PHY_CTR_FUNC_OFFSET);

		break;
	case 3: /* Host 3 ULPI */
#ifdef CONFIG_MX51
		/* The clock for the USBH3 ULPI port will come externally
		   from the PHY. */
		v = __raw_readl(usbother_base + MXC_USB_CTRL_1_OFFSET);
		__raw_writel(v | MXC_USB_CTRL_UH3_EXT_CLK_EN, usbother_base +
				MXC_USB_CTRL_1_OFFSET);
#endif


		v = __raw_readl(usbother_base + MXC_USBH3CTRL_OFFSET);
		if (flags & MXC_EHCI_POWER_PINS_ENABLED)
			v &= ~MXC_H3_UCTRL_H3PM_BIT; /* HOST3 power mask used */
		else
			v |= MXC_H3_UCTRL_H3PM_BIT; /* HOST3 power mask used */

		__raw_writel(v, usbother_base + MXC_USBH3CTRL_OFFSET);

//		v = __raw_readl(usbother_base + MXC_USB_PHY_CTR_FUNC_OFFSET);
//		if (flags & MXC_EHCI_POWER_PINS_ENABLED)
//			v &= ~MXC_H1_OC_DIS_BIT; /* OC is used */
//		else
//			v |= MXC_H1_OC_DIS_BIT; /* OC is not used */
//		__raw_writel(v, usbother_base + MXC_USB_PHY_CTR_FUNC_OFFSET);

		break;
	}

	return ret;
}

void __board_ehci_hcd_postinit(struct usb_ehci *ehci, int port)
{
}

void board_ehci_hcd_postinit(struct usb_ehci *ehci, int port)
	__attribute((weak, alias("__board_ehci_hcd_postinit")));


static int clks_run = 0;
int ehci_hcd_init(int index, struct ehci_hccr **hccr, struct ehci_hcor **hcor)
{
	struct usb_ehci *ehci;
	u32 reg;
#ifdef CONFIG_MX53
	struct clkctl *sc_regs = (struct clkctl *)CCM_BASE_ADDR;

	reg = __raw_readl(&sc_regs->cscmr1) & ~(1 << 26);
	/* derive USB PHY clock multiplexer from PLL3 */
	reg |= 1 << 26;
	__raw_writel(reg, &sc_regs->cscmr1);
#endif

if (clks_run == 0) {
	set_usboh3_clk();
	enable_usboh3_clk(1);
	set_usb_phy_clk();
	enable_usb_phy1_clk(1);
	enable_usb_phy2_clk(1);
	mdelay(1);
	clks_run = 1;
}

#ifndef CONFIG_MXC_USB_PORT1SC
#define	CONFIG_MXC_USB_PORT1SC	(2 << 30)
#define	CONFIG_MXC_USB_FLAGS1	0
#endif
#ifndef CONFIG_MXC_USB_PORT2SC
#define	CONFIG_MXC_USB_PORT2SC	(2 << 30)
#define	CONFIG_MXC_USB_FLAGS2	0
#endif

	/* Do board specific initialization */
	board_ehci_hcd_init(index);

	ehci = (struct usb_ehci *)(OTG_BASE_ADDR +
		(0x200 * index));
	*hccr = (struct ehci_hccr *)((uint32_t)&ehci->caplength);
	*hcor = (struct ehci_hcor *)((uint32_t)*hccr +
			HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));
	setbits_le32(&ehci->usbmode, CM_HOST);

	switch (index) {
	case 0:
		__raw_writel(CONFIG_MXC_USB_PORTSC, &ehci->portsc);
		setbits_le32(&ehci->portsc, USB_EN);
		mxc_set_usbcontrol(index, CONFIG_MXC_USB_FLAGS);
		break;
	case 1:
		__raw_writel(CONFIG_MXC_USB_PORT1SC, &ehci->portsc);
		setbits_le32(&ehci->portsc, USB_EN);
		mxc_set_usbcontrol(index, CONFIG_MXC_USB_FLAGS1);
		break;
	case 2:
		__raw_writel(CONFIG_MXC_USB_PORT2SC, &ehci->portsc);
		setbits_le32(&ehci->portsc, USB_EN);
		mxc_set_usbcontrol(index, CONFIG_MXC_USB_FLAGS2);
		break;
//	case 3:
//		__raw_writel(CONFIG_MXC_USB_PORT3SC, &ehci->portsc);
//		setbits_le32(&ehci->portsc, USB_EN);
//		mxc_set_usbcontrol(index, CONFIG_MXC_USB_FLAGS3);
//		break;
	}

	/* Do board specific post-initialization */
	board_ehci_hcd_postinit(ehci, index);

	return 0;
}

int ehci_hcd_stop(int index)
{
	return 0;
}
