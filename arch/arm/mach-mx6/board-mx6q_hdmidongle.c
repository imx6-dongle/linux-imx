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

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/nodemask.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/fsl_devices.h>
#include <linux/i2c.h>
#include <linux/i2c/pca953x.h>
#include <linux/ata.h>
#include <linux/regulator/consumer.h>
#include <linux/pmic_external.h>
#include <linux/pmic_status.h>
#include <linux/ipu.h>
#include <linux/mxcfb.h>
#include <linux/pwm_backlight.h>
#include <linux/fec.h>
#include <linux/memblock.h>
#include <linux/gpio.h>
#include <linux/etherdevice.h>
#include <linux/regulator/anatop-regulator.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>
#include <linux/mfd/mxc-hdmi-core.h>

#include <mach/common.h>
#include <mach/hardware.h>
#include <mach/mxc_dvfs.h>
#include <mach/memory.h>
#include <mach/iomux-mx6q.h>
#include <mach/imx-uart.h>
#include <mach/viv_gpu.h>
#include <mach/ipu-v3.h>
#include <mach/mxc_hdmi.h>

#include <asm/irq.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>

#include "usb.h"
#include "devices-imx6q.h"
#include "crm_regs.h"
#include "cpu_op-mx6.h"
#include "board-mx6q_hdmidongle.h"

/* verified GK802 GPIOs */
#define HDMIDONGLE_WL_EN        IMX_GPIO_NR(2, 0)
#define HDMIDONGLE_SD2_CD	IMX_GPIO_NR(6, 11)

extern char *gp_reg_id;
extern char *soc_reg_id;
extern char *pu_reg_id;

static const struct esdhc_platform_data mx6q_hdmidongle_sd2_data __initconst = {
	.cd_gpio = HDMIDONGLE_SD2_CD,
	.keep_power_at_suspend = 1,
	.support_8bit = 0,
	.delay_line = 0,
	.cd_type = ESDHC_CD_CONTROLLER,
};

static const struct esdhc_platform_data mx6q_hdmidongle_sd3_data __initconst = {
	.always_present = 1,
	.keep_power_at_suspend = 1,
	.support_8bit = 0,
	.delay_line = 0,
	.cd_type = ESDHC_CD_CONTROLLER,
};

static const struct anatop_thermal_platform_data
	mx6q_hdmidongle_anatop_thermal_data __initconst = {
		.name = "anatop_thermal",
};

static inline void mx6q_hdmidongle_init_uart(void)
{
	imx6q_add_imx_uart(0, NULL);
	imx6q_add_imx_uart(1, NULL);
	imx6q_add_imx_uart(2, NULL);
	imx6q_add_imx_uart(3, NULL);
}

static struct imxi2c_platform_data mx6q_hdmidongle_i2c_data = {
	.bitrate = 100000,
};

static struct i2c_board_info mxc_i2c1_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("mxc_hdmi_i2c", 0x50),
	},
};
static void __init imx6q_hdmidongle_init_usb(void)
{
	imx_otg_base = MX6_IO_ADDRESS(MX6Q_USB_OTG_BASE_ADDR);
	mxc_iomux_set_gpr_register(1, 13, 1, 1);
}


static struct viv_gpu_platform_data imx6q_gpu_pdata __initdata = {
	.reserved_mem_size = SZ_128M + SZ_64M,
};


static struct ipuv3_fb_platform_data hdmidongle_fb_data[] = {
	{/*fb0*/
		.disp_dev = "hdmi",
		.interface_pix_fmt = IPU_PIX_FMT_RGB32,
		.mode_str = "1280x720M@60",
		.default_bpp = 32,
		.int_clk = false,
	},
};

static void hdmi_init(int ipu_id, int disp_id)
{
	int hdmi_mux_setting;

	if ((ipu_id > 1) || (ipu_id < 0)) {
		pr_err("Invalid IPU select for HDMI: %d. Set to 0\n", ipu_id);
		ipu_id = 0;
	}

	if ((disp_id > 1) || (disp_id < 0)) {
		pr_err("Invalid DI select for HDMI: %d. Set to 0\n", disp_id);
		disp_id = 0;
	}

	/* Configure the connection between IPU1/2 and HDMI */
	hdmi_mux_setting = 2*ipu_id + disp_id;

	/* GPR3, bits 2-3 = HDMI_MUX_CTL */
	mxc_iomux_set_gpr_register(3, 2, 2, hdmi_mux_setting);
	/* Set HDMI event as SDMA event2 while Chip version later than TO1.2 */
	if (hdmi_SDMA_check())
		mxc_iomux_set_gpr_register(0, 0, 1, 1);
}

/* On mx6x sabresd board i2c2 iomux with hdmi ddc,
 * the pins default work at i2c2 function,
 when hdcp enable, the pins should work at ddc function */

static void hdmi_enable_ddc_pin(void)
{
	mxc_iomux_v3_setup_multiple_pads(mx6q_hdmidongle_hdmi_ddc_pads,
					ARRAY_SIZE(mx6q_hdmidongle_hdmi_ddc_pads));
}

static void hdmi_disable_ddc_pin(void)
{
	mxc_iomux_v3_setup_multiple_pads(mx6q_hdmidongle_i2c2_pads,
					ARRAY_SIZE(mx6q_hdmidongle_i2c2_pads));
}


static struct fsl_mxc_hdmi_platform_data hdmi_data = {
	.init = hdmi_init,
	.enable_pins = hdmi_enable_ddc_pin,
	.disable_pins = hdmi_disable_ddc_pin,
};

static struct fsl_mxc_hdmi_core_platform_data hdmi_core_data = {
	.ipu_id = 0,
	.disp_id = 0,
};

static struct imx_ipuv3_platform_data ipu_data[] = {
	{
		.rev = 4,
		.csi_clk[0] = "clko_clk",
	}, {
		.rev = 4,
		.csi_clk[0] = "clko_clk",
	},
};

#if defined(CONFIG_ION)
static struct ion_platform_data imx_ion_data = {
	.nr = 1,
	.heaps = {
		{
			.id = 0,
			.type = ION_HEAP_TYPE_CARVEOUT,
			.name = "vpu_ion",
			.size = SZ_64M,
		},
	},
};
#endif

static void hdmidongle_suspend_enter(void)
{
	/* suspend preparation */
}

static void hdmidongle_suspend_exit(void)
{
	/* resume restore */
}
static const struct pm_platform_data mx6q_hdmidongle_pm_data __initconst = {
	.name = "imx_pm",
	.suspend_enter = hdmidongle_suspend_enter,
	.suspend_exit = hdmidongle_suspend_exit,
};

static struct regulator_consumer_supply hdmidongle_vmmc_consumers[] = {
	REGULATOR_SUPPLY("vmmc", "sdhci-esdhc-imx.1"),
	REGULATOR_SUPPLY("vmmc", "sdhci-esdhc-imx.2"),
	REGULATOR_SUPPLY("vmmc", "sdhci-esdhc-imx.3"),
};

static struct regulator_init_data hdmidongle_vmmc_init = {
	.num_consumer_supplies = ARRAY_SIZE(hdmidongle_vmmc_consumers),
	.consumer_supplies = hdmidongle_vmmc_consumers,
};

static struct fixed_voltage_config hdmidongle_vmmc_reg_config = {
	.supply_name		= "vmmc",
	.microvolts		= 3300000,
	.gpio			= -1,
	.init_data		= &hdmidongle_vmmc_init,
};

static struct platform_device hdmidongle_vmmc_reg_devices = {
	.name	= "reg-fixed-voltage",
	.id	= 3,
	.dev	= {
		.platform_data = &hdmidongle_vmmc_reg_config,
	},
};

static struct mxc_dvfs_platform_data hdmidongle_dvfscore_data = {
	.reg_id = "cpu_vddgp",
	.soc_id = "cpu_vddsoc",
	.pu_id = "cpu_vddvpu",
	.clk1_id = "cpu_clk",
	.clk2_id = "gpc_dvfs_clk",
	.gpc_cntr_offset = MXC_GPC_CNTR_OFFSET,
	.ccm_cdcr_offset = MXC_CCM_CDCR_OFFSET,
	.ccm_cacrr_offset = MXC_CCM_CACRR_OFFSET,
	.ccm_cdhipr_offset = MXC_CCM_CDHIPR_OFFSET,
	.prediv_mask = 0x1F800,
	.prediv_offset = 11,
	.prediv_val = 3,
	.div3ck_mask = 0xE0000000,
	.div3ck_offset = 29,
	.div3ck_val = 2,
	.emac_val = 0x08,
	.upthr_val = 25,
	.dnthr_val = 9,
	.pncthr_val = 33,
	.upcnt_val = 10,
	.dncnt_val = 10,
	.delay_time = 80,
};

static void __init fixup_mxc_board(struct machine_desc *desc, struct tag *tags,
				   char **cmdline, struct meminfo *mi)
{
	char *str;
	struct tag *t;
	int i = 0;
	struct ipuv3_fb_platform_data *pdata_fb = hdmidongle_fb_data;

	for_each_tag(t, tags) {
		if (t->hdr.tag == ATAG_CMDLINE) {
			str = t->u.cmdline.cmdline;
			str = strstr(str, "fbmem=");
			if (str != NULL) {
				str += 6;
				pdata_fb[i++].res_size[0] = memparse(str, &str);
				while (*str == ',' &&
					i < ARRAY_SIZE(hdmidongle_fb_data)) {
					str++;
					pdata_fb[i++].res_size[0] = memparse(str, &str);
				}
			}
			break;
		}
	}
}

#define SNVS_LPCR 0x38
static void mx6_snvs_poweroff(void)
{
	void __iomem *mx6_snvs_base =  MX6_IO_ADDRESS(MX6Q_SNVS_BASE_ADDR);
	u32 value;
	value = readl(mx6_snvs_base + SNVS_LPCR);
	/*set TOP and DP_EN bit*/
	writel(value | 0x60, mx6_snvs_base + SNVS_LPCR);
}

/*!
 * Board specific initialization.
 */
static void __init mx6_hdmidongle_board_init(void)
{
	int i;

	mxc_iomux_v3_setup_multiple_pads(mx6q_hdmidongle_pads,ARRAY_SIZE(mx6q_hdmidongle_pads));

	gp_reg_id = hdmidongle_dvfscore_data.reg_id;
	soc_reg_id = hdmidongle_dvfscore_data.soc_id;
	pu_reg_id = hdmidongle_dvfscore_data.pu_id;

	mx6q_hdmidongle_init_uart();

	imx6q_add_mxc_hdmi_core(&hdmi_core_data);

	imx6q_add_ipuv3(0, &ipu_data[0]);
	imx6q_add_ipuv3(1, &ipu_data[1]);

	for (i = 0; i < ARRAY_SIZE(hdmidongle_fb_data); i++)
		imx6q_add_ipuv3fb(i, &hdmidongle_fb_data[i]);

	imx6q_add_vdoa();

	imx6q_add_v4l2_output(0);

	imx6q_add_imx_snvs_rtc();

	imx6q_add_imx_caam();

	imx6q_add_imx_i2c(1, &mx6q_hdmidongle_i2c_data);
	imx6q_add_imx_i2c(2, &mx6q_hdmidongle_i2c_data);
    
	i2c_register_board_info(1, mxc_i2c1_board_info,
			ARRAY_SIZE(mxc_i2c1_board_info));


	imx6q_add_mxc_hdmi(&hdmi_data);

	imx6q_add_anatop_thermal_imx(1, &mx6q_hdmidongle_anatop_thermal_data);
	imx6q_add_pm_imx(0, &mx6q_hdmidongle_pm_data);


	imx6q_add_sdhci_usdhc_imx(3, &mx6q_hdmidongle_sd3_data);
	imx6q_add_sdhci_usdhc_imx(2, &mx6q_hdmidongle_sd2_data);

	imx_add_viv_gpu(&imx6_gpu_data, &imx6q_gpu_pdata);
	imx6q_hdmidongle_init_usb();

	imx6q_add_vpu();

	platform_device_register(&hdmidongle_vmmc_reg_devices);

	imx6q_add_otp();
	imx6q_add_viim();
	imx6q_add_imx2_wdt(0, NULL);
	imx6q_add_dma();

	imx6q_add_dvfs_core(&hdmidongle_dvfscore_data);

#if defined(CONFIG_ION)
	imx6q_add_ion(0, &imx_ion_data,
				  sizeof(imx_ion_data) + sizeof(struct ion_platform_heap));
#endif

	imx6q_add_hdmi_soc();
	imx6q_add_hdmi_soc_dai();


	gpio_request(HDMIDONGLE_WL_EN, "wl_en");
	gpio_direction_output(HDMIDONGLE_WL_EN, 1);
	gpio_set_value(HDMIDONGLE_WL_EN, 0);

	pm_power_off = mx6_snvs_poweroff;
	imx6q_add_busfreq();
}

extern void __iomem *twd_base;
static void __init mx6_hdmidongle_timer_init(void)
{
	struct clk *uart_clk;
#ifdef CONFIG_LOCAL_TIMERS
	twd_base = ioremap(LOCAL_TWD_ADDR, SZ_256);
	BUG_ON(!twd_base);
#endif
	mx6_clocks_init(32768, 24000000, 0, 0);

	uart_clk = clk_get_sys("imx-uart.0", NULL);
	early_console_setup(UART4_BASE_ADDR, uart_clk);
}

static struct sys_timer mx6_hdmidongle_timer = {
	.init   = mx6_hdmidongle_timer_init,
};

static void __init mx6q_hdmidongle_reserve(void)
{
	phys_addr_t phys;
	int i;

#if defined(CONFIG_MXC_GPU_VIV) || defined(CONFIG_MXC_GPU_VIV_MODULE)
	if (imx6q_gpu_pdata.reserved_mem_size) {
		phys = memblock_alloc_base(imx6q_gpu_pdata.reserved_mem_size,
								   SZ_4K, SZ_1G);
		memblock_remove(phys, imx6q_gpu_pdata.reserved_mem_size);
		imx6q_gpu_pdata.reserved_mem_base = phys;
	}
#endif
#if defined(CONFIG_ION)
	if (imx_ion_data.heaps[0].size) {
		phys = memblock_alloc(imx_ion_data.heaps[0].size, SZ_4K);
		memblock_remove(phys, imx_ion_data.heaps[0].size);
		imx_ion_data.heaps[0].base = phys;
	}
#endif

	for (i = 0; i < ARRAY_SIZE(hdmidongle_fb_data); i++)
		if (hdmidongle_fb_data[i].res_size[0]) {
			/* reserve for background buffer */
			phys = memblock_alloc(hdmidongle_fb_data[i].res_size[0],
						SZ_4K);
			memblock_remove(phys, hdmidongle_fb_data[i].res_size[0]);
			hdmidongle_fb_data[i].res_base[0] = phys;
		}
}

/*
 * initialize __mach_desc_MX6Q_HDMIDONGLE data structure.
 */
MACHINE_START(MX6Q_HDMIDONGLE, "Freescale i.MX6q Hi802/GK802 board")
	/* Maintainer: Dmitriy Beykun */
	.boot_params = MX6_PHYS_OFFSET + 0x100,
	.fixup = fixup_mxc_board,
	.map_io = mx6_map_io,
	.init_irq = mx6_init_irq,
	.init_machine = mx6_hdmidongle_board_init,
	.timer = &mx6_hdmidongle_timer,
	.reserve = mx6q_hdmidongle_reserve,
MACHINE_END
