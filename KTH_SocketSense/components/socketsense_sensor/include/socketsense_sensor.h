/**
 * @file socketsense_sensor.h
 * @brief Component that interfaces with the SocketSense Sensor Strips, based on the MPC3208.
 *
 * Each sensor strip is connected to one MCP3208. This component reads values of each of the
 * connected sensor strips.
 *
 * @author Matthias Becker
 * @date June 12. 2019
 */
#ifndef COMPONENTS_SOCKETSENSE_SENSOR_SOCKETSENSE_SENSOR_H_
#define COMPONENTS_SOCKETSENSE_SENSOR_SOCKETSENSE_SENSOR_H_

#include "driver/spi_master.h"

/**
 * @brief This function initializes the sensors
 * @param _spi Handle to the SPI device that is used to communicate with the sensors.
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t socketsense_sensor_init(spi_device_handle_t _spi);

/**
 * @brief This function reads all sensor elements.
 *
 * The function sequentially reads all sensor elements that are present.
 * This is done over the provided SPI device handle. As there are potentially many
 * sensor strips, the chip select line is handled manually by this component.
 * This means, the function sets the respective CS line low/high using the GPIO functionality.
 *
 * @param sensor_data Pointer to the array that is to be filled with the sensor data.
 */
void socketsense_sensor_readSensorData(uint16_t sensor_data[][CONFIG_SOCKETSENSE_SENSEL_COUNT]);

#endif /* COMPONENTS_SOCKETSENSE_SENSOR_SOCKETSENSE_SENSOR_H_ */
