/* arch/arm/mach-msm/board-semc_fuji-oneseg.c
 *
 * Copyright (C) 2011 Sony Ericsson Mobile Communications AB.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/err.h>
#include <linux/regulator/consumer.h>
#include <mach/rpm-regulator.h>

#include <linux/oneseg_tunerpm.h>

#define D_ONESEG_DEVICE_PORT_RESET	27 /* tuner HW reset */

static int oneseg_tunerpm_init(void)
{
	int ret;

	ret = gpio_request(D_ONESEG_DEVICE_PORT_RESET, "Oneseg tuner HW reset");
	if (ret) {
		pr_err("RST request %d\n", ret);
		goto err_request_gpio_rst_req;
	}
	ret = gpio_direction_output(D_ONESEG_DEVICE_PORT_RESET, 0);
	if (ret) {
		pr_err("RST status %d\n", ret);
		goto err_request_gpio_rst;
	}
	return ret;

err_request_gpio_rst:
	gpio_free(D_ONESEG_DEVICE_PORT_RESET);
err_request_gpio_rst_req:
	return -EIO;
}

static int oneseg_tunerpm_free(void)
{
	gpio_free(D_ONESEG_DEVICE_PORT_RESET);
	return 0;
}

static void oneseg_tunerpm_power_control(int on)
{
	int r;

	if (on) {
		r = rpm_vreg_set_voltage(RPM_VREG_ID_PM8058_L25,
						RPM_VREG_VOTER3, 1200000, 1);
		if (r) {
			pr_err("%s: Enable PM8058_L25 error=%d\n",
				__func__, r);
			return;
		}
		r = rpm_vreg_set_voltage(RPM_VREG_ID_PM8901_L4,
						RPM_VREG_VOTER3, 2800000, 1);
		if (r) {
			pr_err("%s: Enable PM8901_L4 error=%d\n",
				__func__, r);
			r = rpm_vreg_set_voltage(RPM_VREG_ID_PM8058_L25,
							RPM_VREG_VOTER3, 0, 1);
			if (r)
				pr_warning("%s: Disable PM8058_L25 error=%d\n",
						__func__, r);
		}
	} else {
		r = rpm_vreg_set_voltage(RPM_VREG_ID_PM8058_L25,
						RPM_VREG_VOTER3, 0, 1);
		if (r)
			pr_warning("%s: Disable PM8058_L25 error=%d\n",
					__func__, r);
		r = rpm_vreg_set_voltage(RPM_VREG_ID_PM8901_L4,
						RPM_VREG_VOTER3, 0, 1);
		if (r)
			pr_warning("%s: Disable PM8901_L4 error=%d\n",
					__func__, r);
	}
}

static void oneseg_tunerpm_reset_control(int on)
{
	gpio_set_value(D_ONESEG_DEVICE_PORT_RESET, on);
}

static struct oneseg_tunerpm_platform_data oneseg_tunerpm_data = {
	.init = oneseg_tunerpm_init,
	.free = oneseg_tunerpm_free,
	.reset_control = oneseg_tunerpm_reset_control,
	.power_control = oneseg_tunerpm_power_control,
};

struct platform_device oneseg_tunerpm_device = {
	.name = D_ONESEG_TUNERPM_DRIVER_NAME,
	.id = 0,
	.dev  = {
		.platform_data = &oneseg_tunerpm_data,
	},
};
