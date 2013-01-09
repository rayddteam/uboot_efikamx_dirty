/*
 * Copyright (C) 2010 Marek Vasut <marek.vasut@gmail.com>
 *
 * (C) Copyright 2009 Freescale Semiconductor, Inc.
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

#include <common.h>
#include <usb.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx5x_pins.h>
#include <asm/arch/iomux.h>
#include <asm/gpio.h>
#include <usb/ehci-fsl.h>
#include <usb/ulpi.h>
#include <errno.h>

#include "../../drivers/usb/host/ehci.h"

#define EFIKASB_LID_SWITCH	MX51_PIN_CSI1_VSYNC
#define EFIKASB_RFKILL_SWITCH	MX51_PIN_DI1_PIN12
#define EFIKASB_BATTERY_LOW	MX51_PIN_DI1_PIN11
#define EFIKASB_BATTERY_INSERT	MX51_PIN_DISPB2_SER_DIO
#define EFIKASB_AC_INSERT	MX51_PIN_DI1_D0_CS

/* active low just like on the board */
#define LID_CLOSED	0
#define WIFI_ON		0
#define BATTERY_IN	0
#define BATTERY_LOW	0
#define AC_IN		0

//struct mxc_iomux_pin_cfg __initdata mx51_efikasb_input_iomux_pins[] = {
//	{ EFIKASB_LID_SWITCH, IOMUX_CONFIG_GPIO, },
//	{ EFIKASB_RFKILL_SWITCH, IOMUX_CONFIG_GPIO, },
//	{ EFIKASB_BATTERY_INSERT, IOMUX_CONFIG_GPIO, },
//	{ EFIKASB_BATTERY_LOW, IOMUX_CONFIG_GPIO | IOMUX_CONFIG_SION, },
//	{ EFIKASB_AC_INSERT, IOMUX_CONFIG_GPIO | IOMUX_CONFIG_SION,

/* USB pin configuration */
#define USB_PAD_CONFIG	(PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST | \
			PAD_CTL_DRV_HIGH | PAD_CTL_100K_PU | \
			PAD_CTL_HYS_ENABLE | PAD_CTL_PUE_PULL)

/*
 * Configure the USB H1 and USB H2 IOMUX
 */
void setup_iomux_usb(void)
{
	setup_iomux_usb_h1();

	if (machine_is_efikasb())
		setup_iomux_usb_h2();

#define EFIKASB_BT_POWER	MX51_PIN_EIM_EB2	/* GPIO1 22 */
#define EFIKASB_WIFI_RESET	MX51_PIN_EIM_D24	/* GPIO1 8 */
#define EFIKASB_WIFI_POWER	MX51_PIN_EIM_EB3	/* GPIO1 23 */
#define EFIKASB_WWAN_WAKEUP	MX51_PIN_CSI1_HSYNC	/* GPIO2 15 */
#define EFIKASB_WWAN_POWER	MX51_PIN_CSI2_D13	/* GPIO3 10 */
#define EFIKASB_WWAN_SIM	MX51_PIN_EIM_CS1	/* GPIO1 26 */
#define EFIKASB_CAMERA_POWER	MX51_PIN_NANDF_CS0	/* GPIO2 16 */

	/* USB PHY reset */
	mxc_request_iomux(MX51_PIN_EIM_D27, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_EIM_D27, PAD_CTL_PKE_ENABLE |
			PAD_CTL_SRE_FAST | PAD_CTL_DRV_HIGH);

	/* USB HUB reset */
	mxc_request_iomux(MX51_PIN_GPIO1_5, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_GPIO1_5, PAD_CTL_PKE_ENABLE |
			PAD_CTL_SRE_FAST | PAD_CTL_DRV_HIGH);

#if 0
	/* WIFI EN (act low) */
	mxc_request_iomux(MX51_PIN_EIM_A22, IOMUX_CONFIG_GPIO);
	mxc_iomux_set_pad(MX51_PIN_EIM_A22, 0);
	/* WIFI RESET */
	mxc_request_iomux(MX51_PIN_EIM_A16, IOMUX_CONFIG_GPIO);
	mxc_iomux_set_pad(MX51_PIN_EIM_A16, 0);
	/* BT EN (act low) */
	mxc_request_iomux(MX51_PIN_EIM_A17, IOMUX_CONFIG_GPIO);
	mxc_iomux_set_pad(MX51_PIN_EIM_A17, 0);
#else
	mxc_request_iomux(EFIKASB_BT_POWER, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(EFIKASB_BT_POWER, USB_PAD_CONFIG);
	mxc_request_iomux(EFIKASB_WIFI_RESET, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(EFIKASB_WIFI_RESET, USB_PAD_CONFIG);
	mxc_request_iomux(EFIKASB_WIFI_POWER, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(EFIKASB_WIFI_POWER, USB_PAD_CONFIG);
	mxc_request_iomux(EFIKASB_WWAN_POWER, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(EFIKASB_WWAN_POWER, USB_PAD_CONFIG);
	mxc_request_iomux(EFIKASB_CAMERA_POWER, IOMUX_CONFIG_GPIO);
	mxc_iomux_set_pad(EFIKASB_CAMERA_POWER, USB_PAD_CONFIG);

//#define EFIKAMX_WIFI_POWER	MX51_PIN_EIM_A22	/* GPIO1 16 */
//#define EFIKAMX_WIFI_RESET	MX51_PIN_EIM_A16	/* GPIO1 10 */
//#define EFIKAMX_BT_POWER	MX51_PIN_EIM_A17	/* GPIO1 11 */

//	mxc_request_iomux(EFIKAMX_BT_POWER, IOMUX_CONFIG_GPIO);
//	mxc_iomux_set_pad(EFIKAMX_BT_POWER, 0);
//	mxc_request_iomux(EFIKAMX_WIFI_RESET, IOMUX_CONFIG_GPIO);
//	mxc_iomux_set_pad(EFIKAMX_WIFI_RESET, 0);
//	mxc_request_iomux(EFIKAMX_WIFI_POWER, IOMUX_CONFIG_GPIO);
//	mxc_iomux_set_pad(EFIKAMX_WIFI_POWER, 0);

#endif
}

/*
 * Enable devices connected to USB BUSes
 */
static void efika_usb_enable_devices(void)
{
#if 0
	/* Enable Bluetooth */
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_EIM_A17), 0);
	udelay(10000);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_EIM_A17), 1);

	/* Enable WiFi */
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_EIM_A22), 1);
	udelay(10000);

	/* Reset the WiFi chip */
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_EIM_A16), 0);
	udelay(10000);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_EIM_A16), 1);
#else
	/* Enable Bluetooth */
	gpio_direction_output(IOMUX_TO_GPIO(EFIKASB_BT_POWER), 0);
	udelay(10000);
	gpio_set_value(IOMUX_TO_GPIO(EFIKASB_BT_POWER), 1);

	/* Enable WiFi */
	gpio_direction_output(IOMUX_TO_GPIO(EFIKASB_WIFI_POWER), 1);
	gpio_direction_output(IOMUX_TO_GPIO(EFIKASB_WWAN_POWER), 0);
	gpio_direction_output(IOMUX_TO_GPIO(EFIKASB_CAMERA_POWER), 0);
	udelay(10000);

	/* Reset the WiFi chip */
	gpio_direction_output(IOMUX_TO_GPIO(EFIKASB_WIFI_RESET), 0);
	udelay(10000);
	gpio_set_value(IOMUX_TO_GPIO(EFIKASB_WIFI_RESET), 1);
#endif

}

/*
 * Reset USB HUB (or HUBs on EfikaSB)
 */
static void efika_usb_hub_reset(void)
{
	/* HUB reset */
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_GPIO1_5), 1);
	udelay(1000);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_GPIO1_5), 0);
	udelay(1000);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_GPIO1_5), 1);
}

/*
 * Reset USB PHY (or PHYs on EfikaSB)
 */
static void efika_usb_phy_reset(void)
{
	/* SMSC 3317 PHY reset */
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_EIM_D27), 0);
	udelay(1000);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_EIM_D27), 1);
}

static void efika_ehci_init(struct usb_ehci *ehci, uint32_t stp_gpio,
				uint32_t alt0, uint32_t alt1)
{
	int ret;
	struct ulpi_regs *ulpi = (struct ulpi_regs *)0;
	struct ulpi_viewport ulpi_vp;

	mxc_request_iomux(stp_gpio, alt0);
	mxc_iomux_set_pad(stp_gpio, PAD_CTL_DRV_HIGH |
				PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST);
	gpio_direction_output(IOMUX_TO_GPIO(stp_gpio), 0);
	udelay(1000);
	gpio_set_value(IOMUX_TO_GPIO(stp_gpio), 1);
	udelay(1000);

	mxc_request_iomux(stp_gpio, alt1);
	mxc_iomux_set_pad(stp_gpio, USB_PAD_CONFIG);
	udelay(10000);

	ulpi_vp.viewport_addr = (u32)&ehci->ulpi_viewpoint;
	ulpi_vp.port_num = 0;

	ret = ulpi_init(&ulpi_vp);
	if (ret) {
		printf("Efika USB ULPI initialization failed\n");
		return;
	}

	/* ULPI set flags */
	ulpi_write(&ulpi_vp, &ulpi->otg_ctrl,
			ULPI_OTG_DP_PULLDOWN | ULPI_OTG_DM_PULLDOWN |
			ULPI_OTG_EXTVBUSIND);
	ulpi_write(&ulpi_vp, &ulpi->function_ctrl,
			ULPI_FC_FULL_SPEED | ULPI_FC_OPMODE_NORMAL |
			ULPI_FC_SUSPENDM);
	ulpi_write(&ulpi_vp, &ulpi->iface_ctrl, 0);

	/* Set VBus */
	ulpi_write(&ulpi_vp, &ulpi->otg_ctrl_set,
			ULPI_OTG_DRVVBUS | ULPI_OTG_DRVVBUS_EXT);

	/*
	 * Set VBusChrg
	 *
	 * NOTE: This violates USB specification, but otherwise, USB on Efika
	 * doesn't work.
	 */
	ulpi_write(&ulpi_vp, &ulpi->otg_ctrl_set, ULPI_OTG_CHRGVBUS);
}

int board_ehci_hcd_init(int port)
{
	if (port > 0)
		return 0;

	efika_usb_enable_devices();
	efika_usb_hub_reset();
	efika_usb_phy_reset();

	return 0;
}

void ehci_powerup_fixup(uint32_t *status_reg, uint32_t *reg)
{
//	uint32_t port = OTG_BASE_ADDR + (0x200 * p);
	uint32_t port = ((uint32_t)status_reg) & 0xfffffe00;
	struct usb_ehci *ehci = (struct usb_ehci *)port;
	struct ulpi_regs *ulpi = (struct ulpi_regs *)0;
	struct ulpi_viewport ulpi_vp;

	ulpi_vp.viewport_addr = (u32)&ehci->ulpi_viewpoint;
	ulpi_vp.port_num = 0;

	ulpi_write(&ulpi_vp, &ulpi->otg_ctrl_set, ULPI_OTG_CHRGVBUS);

	mdelay(50);

	/* terminate the reset */
	*reg = ehci_readl(status_reg);
	*reg |= EHCI_PS_PE;
}

void board_ehci_hcd_postinit(struct usb_ehci *ehci, int port)
{
	uint32_t tmp;

	if (port == 0) {
		/* Adjust UTMI PHY frequency to 24MHz */
		tmp = readl(OTG_BASE_ADDR + 0x80c);
		tmp = (tmp & ~0x3) | 0x01;
		writel(tmp, OTG_BASE_ADDR + 0x80c);
	} else if (port == 1) {
		efika_ehci_init(ehci, MX51_PIN_USBH1_STP,
				IOMUX_CONFIG_ALT2, IOMUX_CONFIG_ALT0);
	} else if ((port == 2)){// && machine_is_efikasb()) {
		efika_ehci_init(ehci, MX51_PIN_EIM_A26,
				IOMUX_CONFIG_ALT1, IOMUX_CONFIG_ALT2);
	}

	if (port)
		mdelay(100);
}

/*
 * Ethernet on the Smarttop is on the USB bus. Rather than give an error about
 * "CPU Net Initialization Failed", just pass this test since no other settings
 * are required. Smartbook doesn't have built-in Ethernet but we will let it
 * pass anyway considering someone may have plugged in a USB stick and all
 * they need to do is run "usb start".
 */
int board_eth_init(bd_t *bis)
{
	return 0;
}
