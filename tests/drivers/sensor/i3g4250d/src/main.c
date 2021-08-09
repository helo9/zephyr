/* On device tests for I3G4250D gyro driver
 *
 * Copyright (c) 2021 Jonathan Hahn
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <ztest.h>
#include <device.h>
#include <drivers/sensor.h>

#define SAMPLING_FREQUENCY 200.0

const struct device *dev;

const struct device *get_sensor_devcie(void)
{
	const struct device *dev = device_get_binding(DT_LABEL(DT_INST(0, st_i3g4250d)));

	zassert_true(device_is_ready(dev), "st i3g4250d not found");

	return dev;
}

static void test_sensor_initialization(void)
{
	const struct device *dev = get_sensor_devcie();

	/* check driver initialization */
	zassert_not_null(dev, "failed: dev is null.");
}

static void test_get_sensor_value(void)
{
	const struct device *dev = get_sensor_devcie();
	struct sensor_value val[3] = {};

	/* check for fetch values */
	int ret = sensor_sample_fetch_chan(dev, SENSOR_CHAN_ALL);

	zassert_true(ret == 0, "failed fetching value");

	/* check for getting values */
	ret = sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, val);

	zassert_true(ret == 0, "failed to get sensor value");
}

static void test_set_sensor_attr(void)
{
	const struct device *dev = get_sensor_devcie();
	struct sensor_value val = {};

	sensor_value_from_double(&val, SAMPLING_FREQUENCY);

	int ret = sensor_attr_set(dev,
				  SENSOR_CHAN_GYRO_XYZ,
				  SENSOR_ATTR_SAMPLING_FREQUENCY,
				  &val);

	zassert_true(ret == 0, "failed to set sensor attribute");
}

void test_main(void)
{
	ztest_test_suite(test_i34250d,
			 ztest_unit_test(test_sensor_initialization),
			 ztest_unit_test(test_get_sensor_value),
			 ztest_unit_test(test_set_sensor_attr));

	ztest_run_test_suite(test_i34250d);
}
