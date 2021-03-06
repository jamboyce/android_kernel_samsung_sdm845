/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/of.h>
#include <linux/of_gpio.h>
#include <cam_sensor_cmn_header.h>
#include <cam_sensor_util.h>
#include <cam_sensor_io.h>
#include <cam_req_mgr_util.h>
#include "cam_aperture_soc.h"
#include "cam_soc_util.h"

int32_t cam_aperture_parse_dt(struct cam_aperture_ctrl_t *a_ctrl,
	struct device *dev)
{
	int32_t                   rc = 0;
	struct cam_hw_soc_info *soc_info = &a_ctrl->soc_info;
	struct cam_aperture_soc_private *soc_private =
			(struct cam_aperture_soc_private *)a_ctrl->soc_info.soc_private;
		struct cam_sensor_power_ctrl_t	*power_info = &soc_private->power_info;
	struct device_node *of_node = NULL;

	/* Initialize mutex */
	mutex_init(&(a_ctrl->aperture_mutex));

	rc = cam_soc_util_get_dt_properties(soc_info);
	if (rc < 0) {
		CAM_ERR(CAM_APERTURE, "parsing common soc dt(rc %d)", rc);
		return rc;
	}

	of_node = soc_info->dev->of_node;

	if (a_ctrl->io_master_info.master_type == CCI_MASTER) {
		rc = of_property_read_u32(of_node, "cci-master",
			&(a_ctrl->cci_i2c_master));
		CAM_DBG(CAM_APERTURE, "cci-master %d, rc %d",
			a_ctrl->cci_i2c_master, rc);
		if (rc < 0 || a_ctrl->cci_i2c_master >= MASTER_MAX) {
			CAM_ERR(CAM_APERTURE, "Wrong info: dt CCI master:%d",
				a_ctrl->cci_i2c_master);
			rc = -EFAULT;
			return rc;
		}
	}

	if (!soc_info->gpio_data) {
		CAM_INFO(CAM_APERTURE, "No GPIO found");
		rc = 0;
		return rc;
	}

	if (!soc_info->gpio_data->cam_gpio_common_tbl_size) {
		CAM_INFO(CAM_APERTURE, "No GPIO found");
		return -EINVAL;
	}

	rc = cam_sensor_util_init_gpio_pin_tbl(soc_info,
			&power_info->gpio_num_info);

	if ((rc < 0) || (!power_info->gpio_num_info)) {
		CAM_ERR(CAM_APERTURE, "No/Error Aperture GPIOs");
		return -EINVAL;
	}
	return rc;
}
