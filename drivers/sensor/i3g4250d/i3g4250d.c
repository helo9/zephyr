/*
 * Copyright (c) 2021 Jonathan Hahn
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT st_i3g4250

#include <init.h>
#include <sys/byteorder.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "i3g4250d.h"

LOG_MODULE_REGISTER(i3g4250d, CONFIG_SENSOR_LOG_LEVEL);

static int i3g4250d_sample_fetch(const struct device *dev,
					enum sensor_channel chan) {
	// TODO implement
	return 0;
}

static int i3g4250d_channel_get(const struct device *dev,
					enum sensor_channel chan,
					struct sensor_value *val){
	return -ENOTSUP;
}


static const struct sensor_driver_api i3g4250d_driver_api = {
	//.attr_set = i3g4250d_attr_set, TODO?
	.sample_fetch = i3g4250d_sample_fetch,
	.channel_get = i3g4250d_channel_get,
};

static int i3g4250d_init(const struct device *dev) {
	return 0;
}

#define CREATE_I3G4250D_DEVICE(inst)									\
	static struct i3g4250d_data data_##inst = {};						\
	static const struct i3g4250d_device_config config_##inst = {};		\
	DEVICE_DT_INST_DEFINE(inst,											\
						  i3g4250d_init,								\
						  device_pm_control_nop,						\
						  &data_##inst,									\
						  &config_##inst,								\
						  POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,		\
						  &i3g4250d_driver_api);

DT_INST_FOREACH_STATUS_OKAY(CREATE_I3G4250D_DEVICE)
