/*
 * Copyright (c) 2021 Jonathan Hahn
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT st_i3g4250d

#include <logging/log.h>
#include "i3g4250d.h"

#if DT_ANY_INST_ON_BUS_STATUS_OKAY(spi)

#define I3G4250D_SPI_READM	(3 << 6)	/* 0xC0 */
#define I3G4250D_SPI_WRITEM	(1 << 6)	/* 0x40 */

LOG_MODULE_DECLARE(i3g4250d, CONFIG_SENSOR_LOG_LEVEL);

struct spi_config i3g4250d_spi_conf = {
	.frequency = 500000U, //TODO Fix!: DT_INST_PROP(0, spi_max_frequency),
	.operation = (SPI_OP_MODE_MASTER | SPI_MODE_CPOL |
		      SPI_MODE_CPHA | SPI_WORD_SET(8) | SPI_LINES_SINGLE),
	.slave     = 0,
	.cs        = NULL,
};

static int i3g4250d_spi_read(struct i3g4250d_data *sensor_data, uint8_t reg,
                uint8_t *data, uint16_t len) {
    struct spi_config *spi_cfg = &i3g4250d_spi_conf;
    uint8_t buffer_tx[2] = {reg | I3G4250D_SPI_READM, 0};
    const struct spi_buf tx_buf = {
        .buf = buffer_tx,
        .len = 2,
    };
    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1,
    };
    const struct spi_buf rx_buf[2] = {
        {
            .buf = NULL,
            .len = 1,
        },
        {
            .buf = data,
            .len = len,
        }
    };
    const struct spi_buf_set rx = {
        .buffers = rx_buf,
        .count = 2,
    };

    if (spi_transceive(sensor_data->bus, spi_cfg, &tx, &rx)) {
        return -EIO;
    }

    return 0;
}

static int i3g4250d_spi_write(struct i3g4250d_data *sensor_data, uint8_t reg,
                uint8_t *data, uint16_t len) {
    struct spi_config *spi_cfg = &i3g4250d_spi_conf;
    uint8_t buffer_tx[2] = {reg | I3G4250D_SPI_WRITEM, 0};
    const struct spi_buf tx_buf[2] = {
        {
            .buf = buffer_tx,
            .len = 1,
        },
        {
            .buf = data,
            .len = len,
        }
    };
    const struct spi_buf_set tx = {
        .buffers = tx_buf,
        .count = 2,
    };
    
    if (spi_write(sensor_data->bus, spi_cfg, &tx)) {
        return -EIO;
    }

    return 0;
}

stmdev_ctx_t i3g4250d_spi_ctx = {
	.read_reg = (stmdev_read_ptr) i3g4250d_spi_read,
	.write_reg = (stmdev_write_ptr) i3g4250d_spi_write,
};

int i3g4250d_spi_init(const struct device *dev)
{
	struct i3g4250d_data *i3g4250d = dev->data;
	const struct i3g4250d_device_config *cfg = dev->config;

	i3g4250d->bus = device_get_binding(cfg->bus_name);

	if (!i3g4250d->bus) {
		LOG_DBG("spi bus not found: %s", cfg->bus_name);
		return -EINVAL;
	}

#if DT_INST_SPI_DEV_HAS_CS_GPIOS(0)
	i3g4250d->cs_ctrl.gpio_dev = device_get_binding(DT_INST_SPI_DEV_CS_GPIOS_LABEL(0));

	if (!i3g4250d->cs_ctrl.gpio_dev) {
		LOG_ERR("Uable to get GPIO SPI CS device.");
		return -ENODEV;
	}

	i3g4250d->cs_ctrl.gpio_pin = DT_INST_SPI_DEV_CS_GPIOS_PIN(0);
	i3g4250d->cs_ctrl.gpio_dt_flags = GPIO_ACTIVE_LOW; //TODO leave that way?
	i3g4250d->cs_ctrl.delay = 0U;

	i3g4250d_spi_conf.cs = &i3g4250d->cs_ctrl;
#endif

	i3g4250d->ctx = &i3g4250d_spi_ctx;
	i3g4250d->ctx->handle = i3g4250d;

	return 0;
}

#endif