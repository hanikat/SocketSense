/**
 * @file data_collector.h
 * @brief Component that reads sensor data from several sensor types and sends them the storage component(s).
 *
 * The component realizes a periodic task that samples all sensor values.
 * The sensor values that are sampled are:
 * BME280 (temperature, humidity, atmospheric pressure).
 * Sensor Stripes, based on the MCP3208 8-channel 12-bit ADC
 *
 * Each recorded sample is tagged with the current ESP time in UNIX us format.
 *
 * @author Matthias Becker
 * @date June 12. 2019
 */
#ifndef COMPONENTS_DATA_COLLECTOR_H_
#define COMPONENTS_DATA_COLLECTOR_H_

/**
 * @brief The define sets the CPU on which the data collector task is statically assigned (can be 0 or 1).
 */
#define DATA_COLLECTOR_CPU 1

/**
 * @brief The define sets the period in ms of the data collector task, and thus the system sampling frequency.
 */
#define DATA_COLLECTOR_TASK_PERIOD_MS 5000

/**
 * @brief Notification values for the data collector task.
 *
 * This is used as notification value when a task notification is sent.
 * We use this inter-task communication mechanism to signal when the task should suspend itself.
 * See: https://www.freertos.org/RTOS-task-notifications.html
 */
#define DATA_COLLECTOR_NOTIFY_STOP 	1

/**
 * @brief This function initializes the data collector component.
 *
 * The initialization configures VSPI to be used for the communication with the sensors.
 * All sensors are initialized, and the queue that is used to send data to further components
 * is initialized.
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t data_collector_init();

/**
 * @brief This function is called to start the data collector task
 *
 * The data collector task executes periodically and records data from each sensor.
 * The data collection can be stopped by sending the DATA_COLLECTOR_NOTIFY_STOP notification value to the task.
 * If this has been done, the task needs to be resumed from outside, i.e. from main.
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t data_collector_start();

/**
 * @brief This function returns the task handle to the data collector task.
 * This can be used to send notifications to the data collector task.
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
TaskHandle_t data_collector_getTaskHandle();

/**
 * @brief Function to send the resume signal to the data collector task.
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t data_collector_resume();

/**
 * @brief Function to send the notification that triggers a task suspend in the next period for the data collector task.
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t data_collector_stop();

#endif /* COMPONENTS_DATA_COLLECTOR_H_ */
