/**
 * @file error_handler.h
 * @brief Component that provides the error management.
 *
 * The component has two tasks. One task periodically checks the battery level and sends a warning message of the voltage
 * drops too low, and the other task is activated by error notifications and displays the error code using the on-board LED.
 *
 * @author Matthias Becker
 * @date July 15. 2019
 */
#ifndef COMPONENTS_ERROR_HANDLER_INCLUDE_ERROR_HANDLER_H_
#define COMPONENTS_ERROR_HANDLER_INCLUDE_ERROR_HANDLER_H_

#define SOCKET_SENSE_ERROR_BME280_NOT_FOUND	(1 << 0)
#define SOCKET_SENSE_ERROR_02	(1 << 1)
#define SOCKET_SENSE_ERROR_03	(1 << 2)
#define SOCKET_SENSE_ERROR_04	(1 << 3)
#define SOCKET_SENSE_ERROR_05	(1 << 4)
#define SOCKET_SENSE_ERROR_06	(1 << 5)
#define SOCKET_SENSE_ERROR_07	(1 << 6)
#define SOCKET_SENSE_ERROR_08	(1 << 7)
#define SOCKET_SENSE_ERROR_09	(1 << 8)
#define SOCKET_SENSE_ERROR_10	(1 << 9)
#define SOCKET_SENSE_ERROR_11	(1 << 10)
#define SOCKET_SENSE_ERROR_12	(1 << 11)
#define SOCKET_SENSE_ERROR_13	(1 << 12)
#define SOCKET_SENSE_ERROR_14	(1 << 13)
#define SOCKET_SENSE_ERROR_15	(1 << 14)
#define SOCKET_SENSE_ERROR_16	(1 << 15)

#define SOCKET_SENSE_WARNING_BATTERY_LEVEL_NOT_OK	(1 << 16)
#define SOCKET_SENSE_WARNING_BATTERY_LEVEL_OK	(1 << 17)
#define SOCKET_SENSE_WARNING_3	(1 << 18)
#define SOCKET_SENSE_WARNING_4	(1 << 19)
#define SOCKET_SENSE_WARNING_5	(1 << 20)
#define SOCKET_SENSE_WARNING_6	(1 << 21)
#define SOCKET_SENSE_WARNING_7	(1 << 22)
#define SOCKET_SENSE_WARNING_8	(1 << 23)
#define SOCKET_SENSE_WARNING_9	(1 << 24)
#define SOCKET_SENSE_WARNING_10	(1 << 25)
#define SOCKET_SENSE_WARNING_11	(1 << 26)
#define SOCKET_SENSE_WARNING_12	(1 << 27)
#define SOCKET_SENSE_WARNING_13	(1 << 28)
#define SOCKET_SENSE_WARNING_14	(1 << 29)
#define SOCKET_SENSE_WARNING_15	(1 << 30)
#define SOCKET_SENSE_WARNING_16	(1 << 31)

/**
 * @brief Type used to send error and warning notifications to the error handler task.
 */
typedef uint32_t socketsense_error_t;

/**
 * @brief Period of the battery task. This is the frequency in which we check the battery level.
 */
#define BATTERY_TASK_PERIOD_MS 	30000

/**
 * @brief The number of samples taken for one battery reading. The average value of samples is used to reduce fluctuations.
 */
#define BATTERY_NO_OF_SAMPLES	10

/**
 * @brief Battery warning voltage in mV. If the recorded battery voltage is less than this value the warning LED is turned on.
 */
#define BATTERY_WARNING_VOLTAGE 3300

/**
 * @brief This function initializes the error handling.
 * The error handling is done in two tasks. One periodic task that checks the battery level, and one
 * task that is only active if an error notification has been sent.
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t error_handler_init(void);

/**
 * @brief Function to set error and warning on system level.
 * More than one notification can be used by combining the different error and warning bits in the error code.
 *
 * @param error_code The error code is a 32 bit value, of which the first 16 bit are errors and the later 16 bit are warnings.
 */
void error_handler_notify(socketsense_error_t error_code);

/**
 * @brief This function returns the current battery voltage.
 * Note that this is only the last read value, thus the update interval depends on BATTERY_TASK_PERIOD_MS.
 * @return Last read battery voltage.
 */
uint32_t getBatteryVoltage();

#endif /* COMPONENTS_ERROR_HANDLER_INCLUDE_ERROR_HANDLER_H_ */
