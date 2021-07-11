/* ST Microelectronics I3G4250D gyro driver
 *
 * Copyright (c) 2021 Jonathan Hahn
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Datasheet:
 * https://www.st.com/resource/en/datasheet/i3g4250d.pdf
 */

#define DT_DRV_COMPAT st_i3g4250d

#include <init.h>
#include <sys/__assert.h>
#include <sys/byteorder.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "i3g4250d.h"

LOG_MODULE_REGISTER(i3g4250d, CONFIG_SENSOR_LOG_LEVEL);

static int i3g4250d_sample_fetch(const struct device *dev,
					enum sensor_channel chan) {
	
	struct i3g4250d_data *i3g4250d = dev->data;
	uint8_t reg;
	int16_t buf[3] = {0, 0, 0};

	__ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL || chan == SENSOR_CHAN_GYRO_XYZ);

	i3g4250d_flag_data_ready_get(i3g4250d->ctx, &reg);

	if (reg) {
		if(i3g4250d_angular_rate_raw_get(i3g4250d->ctx, buf) <0) {
			LOG_DBG("Failed to fetch raw data sample!");
			return -EIO;
		};

		i3g4250d->angular_rate[0] = buf[0];
		i3g4250d->angular_rate[1] = buf[1];
		i3g4250d->angular_rate[2] = buf[2];
		
		return 0;
	} else {
		return -EIO;
	}
}

static inline void i3g4250d_convert(struct sensor_value *val, int16_t raw_value) {
	val->val1 = raw_value / 1000;
	val->val2 = (raw_value * 1000) % 1000000;
}

static void i3g4250d_channel_convert(enum sensor_channel chan,
						uint16_t *raw_xyz,
						struct sensor_value *val)
{
	int i;
	uint8_t ofs_start, ofs_stop;

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
		ofs_start = 0U;
		ofs_stop = 2U;
		break;	
	}

	
	for (i = ofs_start; i <= ofs_stop; i++) {
		i3g4250d_convert(val++, raw_xyz[i]);
	}
}

static int i3g4250d_channel_get(const struct device *dev,
					enum sensor_channel chan,
					struct sensor_value *val){
	
	struct i3g4250d_data *i3g4250d = dev->data;


	switch (chan) {
	case SENSOR_CHAN_GYRO_X:
	case SENSOR_CHAN_GYRO_Y:
	case SENSOR_CHAN_GYRO_Z:
	case SENSOR_CHAN_GYRO_XYZ:
		i3g4250d_channel_convert(chan, i3g4250d->angular_rate, val);
		return 0;
	default:
		return -ENOTSUP;
	}
}

static const struct sensor_driver_api i3g4250d_driver_api = {
	//.attr_set = i3g4250d_attr_set, TODO?
	.sample_fetch = i3g4250d_sample_fetch,
	.channel_get = i3g4250d_channel_get,
};

static int i3g4250d_init(const struct device *dev) {

	struct i3g4250d_data *data = dev->data;
	uint8_t wai;

	if (i3g4250d_spi_init(dev)) {
		return -EINVAL;
	}

	if (i3g4250d_device_id_get(data->ctx, &wai) < 0) {
		return -EIO;
	}
	
	if (wai != I3G4250D_ID) {
		LOG_ERR("Inavild chip ID: %02x", wai);
		return -EIO;
	}

	/* Configure filtering chain -  Gyroscope - High Pass */
	i3g4250d_filter_path_set(data->ctx, I3G4250D_LPF1_HP_ON_OUT);
	i3g4250d_hp_bandwidth_set(data->ctx, I3G4250D_HP_LEVEL_3);

	/* Set Output data rate */
	i3g4250d_data_rate_set(data->ctx, I3G4250D_ODR_100Hz);

	return 0;
}

#define I3G4250D_DEVICE_INIT(inst)											\
	static struct i3g4250d_data i3g4250d_data_##inst;						\
	static const struct i3g4250d_device_config i3g4250d_config_##inst = {	\
		.bus_name = DT_INST_BUS_LABEL(inst),								\
	};																		\
	DEVICE_DT_INST_DEFINE(inst,												\
						  i3g4250d_init,									\
						  NULL,												\
						  &i3g4250d_data_##inst,							\
						  &i3g4250d_config_##inst,							\
						  POST_KERNEL, 										\
						  CONFIG_SENSOR_INIT_PRIORITY,						\
						  &i3g4250d_driver_api);

DT_INST_FOREACH_STATUS_OKAY(I3G4250D_DEVICE_INIT)