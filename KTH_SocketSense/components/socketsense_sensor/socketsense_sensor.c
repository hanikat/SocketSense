/**
 * @file socketsense_sensor.c
 * @brief Component that interfaces with the SocketSense Sensor Strips, based on the MPC3208.
 *
 * Each sensor strip is connected to one MCP3208. This component reads values of each of the
 * connected sensor strips.
 *
 * @author Matthias Becker
 * @date June 12. 2019
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_err.h>
#include "esp_system.h"
#include "esp_log.h"

#include "socketsense_sensor.h"
#include "KTHSocketSense.h"

spi_device_handle_t spiHandle;

static const char *TAG = "SOCKETSENSE SENSOR";

#define MAX_SENSOR_STRIPS 4

gpio_num_t cs_line[MAX_SENSOR_STRIPS] = {PIN_NUM_SENSOR_CS1, PIN_NUM_SENSOR_CS2, PIN_NUM_SENSOR_CS3, PIN_NUM_SENSOR_CS4};

/*****Private Functions Definitions*************************************************/

uint16_t socketsense_sensor_read(uint8_t sensorId, uint8_t senselId);

/*****Public Functions**************************************************************/

/**
 * Initialize the sensors. This includes mainly the configuration of
 * GPIO pins that are used as chip select for the individual sensor strips.
 */
esp_err_t socketsense_sensor_init(spi_device_handle_t _spi)
{
	uint8_t i = 0;

	spiHandle = _spi;

	//initialize the GPIO pins we use as chip select
	for(i = 0; i < MAX_SENSOR_STRIPS; i++){
		gpio_pad_select_gpio(cs_line[i]);
		gpio_set_direction(cs_line[i], GPIO_MODE_OUTPUT);
		gpio_set_level(cs_line[i], 1);
	}

	ESP_LOGI(TAG, "Initialized");

	return ESP_OK;
}

/**
 * This function reads all sensors strips and fills in the provided data structure
 */
void socketsense_sensor_readSensorData(uint16_t sensor_data[][CONFIG_SOCKETSENSE_SENSEL_COUNT]){

	uint8_t sensor_id = 0;
	uint8_t sensel_id = 0;

	for(sensor_id = 0; sensor_id < CONFIG_SOCKETSENSE_SENSOR_COUNT; sensor_id++){
		for(sensel_id = 0; sensel_id < CONFIG_SOCKETSENSE_SENSEL_COUNT; sensel_id++){
			sensor_data[sensor_id][sensel_id] = socketsense_sensor_read(sensor_id, sensel_id);
		}
	}
}

/*****Private Functions*************************************************************/

/**
 * This function reads a single sensor element and returns the ADC value
 * Each sensor strip has a MCP3208. One reading can be performed by transmitting 3 bytes.
 */
uint16_t socketsense_sensor_read(uint8_t sensorId, uint8_t senselId){
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length=3 * 8;
	t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
	t.user = (void*)1;
	t.tx_data[0] = (0x01 << 2) | (0x01 << 1) | (senselId >> 2);
	t.tx_data[1] = (senselId << 6);

	gpio_set_level(cs_line[sensorId], 0);
	esp_err_t ret = spi_device_polling_transmit(spiHandle, &t);
	gpio_set_level(cs_line[sensorId], 1);
	assert( ret == ESP_OK );

	return (uint16_t)(((t.rx_data[1] & 0x0F) << 8) | (t.rx_data[2]));
}
