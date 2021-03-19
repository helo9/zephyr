#include <ztest.h>
#include <i3g4250d.h>

static void test_testing()
{
    zassert_true(1, "1 is not true!");
}

static void test_sensor_initialization() 
{
    const struct device *dev;

    dev = device_get_binding(DT_LABEL(DT_INST(0, st_i3g4250d)));

    zassert_not_null(dev, "failed: dev is null.");
}

void test_main(void)
{
    ztest_test_suite(test_i34250d,
                ztest_unit_test(test_testing),
                ztest_unit_test(test_sensor_initialization));
    
    ztest_run_test_suite(test_i34250d);
}