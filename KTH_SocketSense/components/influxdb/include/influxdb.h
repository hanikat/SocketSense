/**
 * @file influxdb.h
 * @brief Component that receives measurement data and transmits it to a InfluxDB instance reachable over the network.
 *
 * This component realizes a periodic task that receives measurement data over a OS queue.
 * Each item is sent to an InfluxDB instance that is reachable over the network.
 * Data is communicated using the line protocol, https://docs.influxdata.com/influxdb/v1.7/write_protocols/line_protocol_reference/
 *
 * Additionally, the same sample is stored on the SD-card (if it was found during boot).
 * The data is stored on the SD-card using the InfluxDB line protocol.
 * Doing this allows to upload the stored data points to the database from SD-card using a PC using the following command:
 * $curl -i -XPOST 'http://localhost:8086/write?db=mydb' --data-binary @@filename.txt`
 *
 * A description is provided here: https://docs.influxdata.com/influxdb/v1.7/guides/writing_data/
 *
 * @author Matthias Becker
 * @date June 12. 2019
 */
#ifndef COMPONENTS_INFLUXDB_H_
#define COMPONENTS_INFLUXDB_H_

/**
 * @brief Initializes the InfluxDB component.
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t influxdb_init();

/**
 * @brief Create the periodic task that receives the measurement data and sends it to the database.
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t influxdb_enable();

/**
 * @brief Delete the influxdb task.
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t influxdb_disable();

/**
 * @brief Does cleanup, this also disables the task.
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t influxdb_deinit();

#endif /* COMPONENTS_INFLUXDB_H_ */
