/* ST Microelectronics I3G4250D gyro driver
 *
 * Copyright (c) 2021 Jonathan Hahn
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Datasheet:
 * https://www.st.com/resource/en/datasheet/i3g4250d.pdf
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_I3G4250D_I3G4250D_H_
#define ZEPHYR_DRIVERS_SENSOR_I3G4250D_I3G4250D_H_

#include <stdint.h>
#include <drivers/spi.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
#include <sys/util.h>
#include "i3g4250d_reg.h"

struct i3g4250d_device_config {
    const char *bus_name;
    uint8_t pm;
};

/* sensor data */
struct i3g4250d_data {
    const struct device *bus;
    int16_t angular_rate[3];

    stmdev_ctx_t *ctx;

#if DT_INST_SPI_DEV_HAS_CS_GPIOS(0)
	struct spi_cs_control cs_ctrl;
#endif
};

int i3g4250d_spi_init(const struct device *dev);

#endif /* __SENSOR_I3G4250D__ */