/*
 * UDOO board power off
 *
 *
 * Copyright (C) 2014 Jasbir Matharu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>

#define SNVS_LPCR 0x04

static int sam3x_rst_gpio,pwr_5v_gpio;

static void udoo_power_off(void) {

	struct device_node *snvs_np;
	void __iomem *mx6_snvs_base;
	u32 value;

	pr_info("Powering off udoo\n");

	snvs_np = of_find_compatible_node(NULL, NULL, "fsl,sec-v4.0-mon-rtc-lp");
	if (!snvs_np) {
		pr_err("%s: failed to find sec-v4.0-mon-rtc-lp node\n",__func__);
		return;
	}

	mx6_snvs_base = of_iomap(snvs_np, 0);
	if (!mx6_snvs_base) {
		pr_err("%s: failed to map sec-v4.0-mon-rtc-lp\n",__func__);
		goto put_snvs_node;
	}

    value = readl(mx6_snvs_base + SNVS_LPCR);
    /*set TOP and DP_EN bit*/
    writel(value | 0x60, mx6_snvs_base + SNVS_LPCR);

	gpio_request_one(sam3x_rst_gpio, GPIOF_OUT_INIT_LOW,"sam3x_rst_gpio"),
	msleep(5);
	gpio_request_one(pwr_5v_gpio, GPIOF_OUT_INIT_HIGH,"pwr_5v_gpio");

put_snvs_node:
	of_node_put(snvs_np);

}

static int udoo_power_off_probe(struct platform_device *pdev)
{
	/* If a pm_power_off function has already been added, leave it alone */
	if (pm_power_off != NULL) {
		pr_err("%s: pm_power_off function already registered",
		       __func__);
		return -EBUSY;
	}

	sam3x_rst_gpio = of_get_named_gpio(pdev->dev.of_node, "sam3x_rst_gpio", 0);
	pwr_5v_gpio = of_get_named_gpio(pdev->dev.of_node, "pwr_5v_gpio", 0);
	if (gpio_is_valid(sam3x_rst_gpio) && gpio_is_valid(pwr_5v_gpio)) {
	} else {
		pr_err("%s : failed to find sam3x_rst_gpio or pwr_5v_gpio property \n",__func__);
		return ENOENT;
	}

	pm_power_off = udoo_power_off;
	pr_info("%s: ok\n",__func__);
	return 0;
}

static int udoo_power_off_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id power_off_dt_ids[] = {
	{ .compatible = "udoo,poweroff", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, power_off_dt_ids);

static struct platform_driver udoo_power_off_driver = {
	.driver = {
		.name	= "udoo_power_off",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(power_off_dt_ids),
	},
	.probe		= udoo_power_off_probe,
	.remove		= udoo_power_off_remove,
};
module_platform_driver(udoo_power_off_driver);

MODULE_AUTHOR("Jasbir Matharu");
MODULE_DESCRIPTION("UDOO Power off driver");
MODULE_LICENSE("GPL v2");
