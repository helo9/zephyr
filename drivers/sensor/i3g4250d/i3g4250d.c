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

static inline void i3g4250d_convert(struct sensor_value *val, int raw_val,
				  uint32_t gain) 
{
	int64_t dval;

	// TODO finish
}

static inline void i3g4250d_channel_get_gyro(const struct device *dev,
					enum sensor_channel chan,
					struct sensor_value *val){
	int i;
	uint8_t ofs_start, ofs_stop;
	struct i3g4250d_data *i3g4250d = dev->data;
	struct sensor_value *pval = val;

	switch (chan) {
		case SENSOR_CHAN_GYRO_X:
			ofs_start = ofs_stop = 0U;
			break;
		case SENSOR_CHAN_GYRO_Y:
			ofs_start = ofs_stop = 1U;
			break;
		case SENSOR_CHAN_GYRO_Z:
			ofs_start = ofs_stop = 2U;
			break;
		default:
			ofs_start = 0U; ofs_stop = 2U;
			break;
	}

	for (int i = ofs_start; i <= ofs_stop; i++) {
		i3g4250d_convert(pval++, i3g4250d->angular_rate[i], 1.0);
	}
}

static int i3g4250d_channel_get(const struct device *dev,
					enum sensor_channel chan,
					struct sensor_value *val){
	switch(chan) {
		case SENSOR_CHAN_GYRO_X:
		case SENSOR_CHAN_GYRO_Y:
		case SENSOR_CHAN_GYRO_Z:
		case SENSOR_CHAN_GYRO_XYZ:
			i3g4250d_channel_get_gyro(dev, chan, val);
			return 0;
		default:
			LOG_DBG("Channel not supported");
			break;
	}

	return -ENOTSUP;
}


static const struct sensor_driver_api i3g4250d_driver_api = {
	//.attr_set = i3g4250d_attr_set, TODO?
	.sample_fetch = i3g4250d_sample_fetch,
	.channel_get = i3g4250d_channel_get,
};

static int i3g4250d_init_interface(const struct device *dev) {
	struct i3g4250d_data *i3g4250d = dev->data;
	const struct i3g4250d_device_config *cfg = dev->config;

	i3g4250d->bus = device_get_binding(cfg->bus_name);
	if (!i3g4250d->bus) {
		LOG_DBG("master bus not found:%s", cfg->bus_name);
		return -EINVAL;
	}

	i3g4250d_spi_init(dev);

	return 0;
}

static int i3g4250d_init(const struct device *dev) {
	struct i3g4250d_data *i3g4250d = dev->data;
	const struct i3g4250d_device_config *cfg = dev->config;
	uint8_t wai; //who am I

	if (i3g4250d_init_interface(dev)) {
		return -EINVAL;
	}

	/* check chip ID */
	i3g4250d_device_id_get(dev->ctx, &wai);

	if (wai != I3G4250D_ID) {
		LOG_ERR("Invalid chip ID: %02x", wai);
		return -EINVAL;
	}

	if(i3g4250d_filter_path_set(&dev_ctx, I3G4250D_LPF1_HP_ON_OUT)){
		return -EIO;
	}

  	if(i3g4250d_hp_bandwidth_set(&dev_ctx, I3G4250D_HP_LEVEL_3)) {
		  return -EIO;
	}

	if(i3g4250d_data_rate_set(&dev_ctx, I3G4250D_ODR_100Hz)) {
		return -EIO;
	}

	return 0;
}

const struct i3g4250d_device_config i3g4250d_cfg = {
	.bus_name = DT_INST_BUS_LABEL(0),
	.pm = 1,
};

struct i3g4250d_data i3g4250d_data;

#ifndef TEST
DEVICE_DT_INST_DEFINE(0, i3g4250d_init, device_pm_control_nop,
		    &i3g4250d_data, &i3g4250d_cfg,
		    POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
		    &i3g4250d_driver_api);

#else

DEVICE_DEFINE(i3g4250d_driver_api, SENSOR_NAME, &i3g4250_init,
			NULL, i3g4250d_data, &i3g4250_cfg, APPLICATION,
			CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &i3g4250d_driver_api);

#endif // TEST