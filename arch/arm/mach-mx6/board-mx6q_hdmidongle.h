/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _BOARD_MX6Q_HDMIDONGLE_H
#define _BOARD_MX6Q_HDMIDONGLE_H
#include <mach/iomux-mx6q.h>

static iomux_v3_cfg_t mx6q_hdmidongle_rev_a_pads[] = {
	/*USB_OTG_DET(USB OTG cable plug detect) */
	MX6Q_PAD_EIM_A16__GPIO_2_22,
	/*BT_WAKEUP_HOST(Combo module BT wake-up output) */
	MX6Q_PAD_EIM_A25__GPIO_5_2,
	/* HOST_WAKEUP_BT(CPU wakeup BT signal)*/
	MX6Q_PAD_EIM_D16__GPIO_3_16,
	
	/* USB OC pin */
	MX6Q_PAD_EIM_D21__USBOH3_USBOTG_OC,

	/* WLAN_WAKEUP_HOST(Combo module WLAN host wake-up output) */
	MX6Q_PAD_EIM_D22__GPIO_3_22,

	/* UARTS */

	/* UART1 for debug */
	MX6Q_PAD_CSI0_DAT10__UART1_TXD,
	MX6Q_PAD_CSI0_DAT11__UART1_RXD,
	
	/* BT_UART2  */
	MX6Q_PAD_EIM_D26__UART2_TXD,
	MX6Q_PAD_EIM_D27__UART2_RXD,
	MX6Q_PAD_EIM_D28__UART2_CTS,
	MX6Q_PAD_EIM_D29__UART2_RTS,
	
	/* UART4 for debug */
	MX6Q_PAD_KEY_COL0__UART4_TXD,
	MX6Q_PAD_KEY_ROW0__UART4_RXD,

	/*BT_nRST(Combo module BT reset signal)*/
	MX6Q_PAD_EIM_DA7__GPIO_3_7,

	/*BT_REG_ON(Combo module BT Internal regulators power enable/disable)*/
	MX6Q_PAD_EIM_DA9__GPIO_3_9,

	/*WL_REG_ON(Combo module WLAN Internal regulators power enable/disable)*/
	MX6Q_PAD_EIM_DA10__GPIO_3_10,

	/* SD SLOTS */
	/* SD1 (Combo module WLAN SDIO )*/
	MX6Q_PAD_SD1_CLK__USDHC1_CLK,
	MX6Q_PAD_SD1_CMD__USDHC1_CMD,
	MX6Q_PAD_SD1_DAT0__USDHC1_DAT0,
	MX6Q_PAD_SD1_DAT1__USDHC1_DAT1,
	MX6Q_PAD_SD1_DAT2__USDHC1_DAT2,
	MX6Q_PAD_SD1_DAT3__USDHC1_DAT3,

	/* SD2 (MicroSD SDIO CMD)*/
	MX6Q_PAD_SD2_CLK__USDHC2_CLK,
	MX6Q_PAD_SD2_CMD__USDHC2_CMD,
	MX6Q_PAD_SD2_DAT0__USDHC2_DAT0,
	MX6Q_PAD_SD2_DAT1__USDHC2_DAT1,
	MX6Q_PAD_SD2_DAT2__USDHC2_DAT2,
	MX6Q_PAD_SD2_DAT3__USDHC2_DAT3,
	/*SD_DET (SD plug-in detect interrupt)*/
	MX6Q_PAD_GPIO_4__GPIO_1_4,

	/* SD3 (eMMC SDIO)*/
	MX6Q_PAD_SD3_CLK__USDHC3_CLK_200MHZ,
	MX6Q_PAD_SD3_CMD__USDHC3_CMD_200MHZ,
	MX6Q_PAD_SD3_DAT0__USDHC3_DAT0_200MHZ,
	MX6Q_PAD_SD3_DAT1__USDHC3_DAT1_200MHZ,
	MX6Q_PAD_SD3_DAT2__USDHC3_DAT2_200MHZ,
	MX6Q_PAD_SD3_DAT3__USDHC3_DAT3_200MHZ,
	MX6Q_PAD_SD3_DAT4__USDHC3_DAT4_200MHZ,
	MX6Q_PAD_SD3_DAT5__USDHC3_DAT5_200MHZ,
	MX6Q_PAD_SD3_DAT6__USDHC3_DAT6_200MHZ,
	MX6Q_PAD_SD3_DAT7__USDHC3_DAT7_200MHZ,
	MX6Q_PAD_SD3_RST__USDHC3_RST,

	/*HDMI CEC communication PIN*/
	MX6Q_PAD_KEY_ROW2__HDMI_TX_CEC_LINE,

	/* I2Cs */

	/* I2C1  */
	MX6Q_PAD_CSI0_DAT8__I2C1_SDA,
	MX6Q_PAD_CSI0_DAT9__I2C1_SCL,

	/* I2C2 */
	MX6Q_PAD_KEY_COL3__I2C2_SCL,	/* GPIO4[12] */
	MX6Q_PAD_KEY_ROW3__I2C2_SDA,	/* GPIO4[13] */

	/* I2C3  */
	MX6Q_PAD_EIM_D17__I2C3_SCL,
	MX6Q_PAD_EIM_D18__I2C3_SDA,

	/*DCDC5V_PWREN(5V DCDC BOOST control signal)*/
	MX6Q_PAD_KEY_COL4__GPIO_4_14,

	/*USB_OTG_PWREN(USB OTG power change control signal)*/
	MX6Q_PAD_KEY_ROW4__GPIO_4_15,

	/* USBOTG ID pin */
	MX6Q_PAD_GPIO_1__USBOTG_ID,

	/*WDOG(Watch dog output)*/
	MX6Q_PAD_GPIO_9__GPIO_1_9,

	/*GPIO_nRST(GPIO shutdown control)*/
	MX6Q_PAD_NANDF_CS1__GPIO_6_14,

	/*PWRKEY_DET(Power key press detection)*/
	MX6Q_PAD_NANDF_CS3__GPIO_6_16,

	/*CHG_SYS_ON(Charger auto power on control signal)*/
	MX6Q_PAD_NANDF_D6__GPIO_2_6,

	/*IR_RC*/
	MX6Q_PAD_SD4_DAT6__GPIO_2_14,
};

static iomux_v3_cfg_t mx6q_hdmidongle_hdmi_ddc_pads[] = {
	MX6Q_PAD_KEY_COL3__HDMI_TX_DDC_SCL, /* HDMI DDC SCL */
	MX6Q_PAD_KEY_ROW3__HDMI_TX_DDC_SDA, /* HDMI DDC SDA */
};

static iomux_v3_cfg_t mx6q_hdmidongle_i2c2_pads[] = {
	MX6Q_PAD_KEY_COL3__I2C2_SCL,	/* I2C2 SCL */
	MX6Q_PAD_KEY_ROW3__I2C2_SDA,	/* I2C2 SDA */
};

#endif
