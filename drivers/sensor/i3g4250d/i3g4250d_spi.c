/*
 * Copyright (c) 2021 Jonathan Hahn
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#define DT_DRV_COMPAT st_iis2dh

#include <logging/log.h>
#include "i3g4250d.h"

#if DT_ANY_INST_ON_BUS_STATUS_OKAY(spi)

#define I3G4250D_SPI_READM	(3 << 6)	/* 0xC0 */ //TODO CHECK!!
#define I3G4250D_SPI_WRITEM	(1 << 6)	/* 0x40 */

LOG_MODULE_DECLARE(IIS2DH, CONFIG_SENSOR_LOG_LEVEL);

static struct spi_config i3g4250d_spi_conf = {
	.frequency = DT_INST_PROP(0, spi_max_frequency),
	.operation = (SPI_OP_MODE_MASTER | SPI_MODE_CPOL |
		      SPI_MODE_CPHA | SPI_WORD_SET(8) | SPI_LINES_SINGLE),
	.slave     = DT_INST_REG_ADDR(0),
	.cs        = NULL,
};

static int i3g4250d_spi_read(struct i3g4250d_data *ctx, uint8_t reg,
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

    if (spi_transceive(ctx->bus, spi_cfg, &tx, &rx)) {
        return -EIO;
    }

    return 0;
}

static int i3g4250d_spi_write(struct i3g4250d_data *ctx, uint8_t reg,
                uint8_t *data, uint16_t len) {
    struct spi_config *spi_cfg = &i3g4250d_spi_conf;
    uint8_t buffer_tx[2] = {reg | I3G4250D_SPI_WRITEM, 0};
    const struct spi_buf tx_buf[0] = {
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
        .buffers = &tx_buf,
        .count = 1,
    };
    
    if (spi_write(ctx->bus, spi_cfg, &tx, &rx)) {
        return -EIO;
    }

    return 0;
}

stmdev_ctx_t i3g4250d_spi_ctx = {
	.read_reg = (stmdev_read_ptr) i3g4250d_spi_read,
	.write_reg = (stmdev_write_ptr) i3g4250d_spi_write,
}

int iis2dh_spi_init(const struct *dev) {
	struct i3g4250d_data *i3g4250d = dev->data;

	data->ctx = &i3g4250_spi_ctx;
	data->ctx->handle = data;

#if DT_INST_SPI_DEV_HAS_CS_GPIOS(0)
    /* handle SPI CS through GPIO if available */
	data->cs_ctrl.gpio_dev = device_get_binding(
		DT_INST_SPI_DEV_CS_GPIOS_LABEL(0));
	if (!data->cs_ctrl.gpio_dev) {
		LOG_ERR("Unable to get GPIO SPI CS device");
		return -ENODEV;
	}

	data->cs_ctrl.gpio_pin = DT_INST_SPI_DEV_CS_GPIOS_PIN(0);
	data->cs_ctrl.delay = 0U;

#endif

	return 0;
}

#endif /* DT_ANY_INST_ON_BUS_STATUS_OKAY(spi) */