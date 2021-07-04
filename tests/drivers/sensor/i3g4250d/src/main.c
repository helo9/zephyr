#include <ztest.h>
#include <device.h>
#include <drivers/sensor.h>

const struct device *dev;

const struct device *get_sensor_devcie(void)
{
	const struct device *dev = device_get_binding(DT_LABEL(DT_INST(0, st_i3g4250d)));

	zassert_true(device_is_ready(dev), "st i3g4250d not found");

	return dev;
}

static void test_sensor_initialization() 
{
    const struct device *dev = get_sensor_devcie();

    zassert_not_null(dev, "failed: dev is null.");
}

static void test_get_sensor_value()
{
    const struct device *dev = get_sensor_devcie();
    struct sensor_value val[3] = {};

    int ret = sensor_sample_fetch_chan(dev, SENSOR_CHAN_ALL);

    zassert_true(ret == 0, "failed fetching value");

    ret = sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, val);

    zassert_true(ret == 0, "failed to get sensor value");
}

void test_main(void)
{
    ztest_test_suite(test_i34250d,
                ztest_unit_test(test_sensor_initialization),
                ztest_unit_test(test_get_sensor_value));
    
    ztest_run_test_suite(test_i34250d);
}