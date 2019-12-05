/**
 * @file KTHSocketSense.h
 * @brief Header file of the application.
 *
 * This header file defines the pin mapping depending on the connected device (the device is selected in the main menuconfig settings of the IDF project).
 * The file further defines the data structure that holds all required sensor values, and it defines the OS queue used to send data from
 * the data collection component to the InfluxDB component.
 *
 * @author Matthias Becker
 * @date June 18. 2019
 */
#ifndef MAIN_INCLUDE_KTHSOCKETSENSE_H_
#define MAIN_INCLUDE_KTHSOCKETSENSE_H_

#include "bme280.h"

/**
 * @brief This is configuring all pin assignments if the adafruit feather board is used
 */
#define PIN_NUM_SENSOR_MISO 		GPIO_NUM_34		//!< MISO pin of the Sensor SPI (VSPI)
#define PIN_NUM_SENSOR_MOSI 		GPIO_NUM_25		//!< MOSI pin of the Sensor SPI (VSPI)
#define PIN_NUM_SENSOR_CLK  		GPIO_NUM_26		//!< Clock pin of the Sensor SPI (VSPI)
#define PIN_NUM_BME280_CS   		GPIO_NUM_4		//!< Chip select pin of the BME280
#define PIN_NUM_PCF8523_SDA			GPIO_NUM_23		//!< SDA pin connected to the RTC
#define PIN_NUM_PCF8523_SCL 		GPIO_NUM_22		//!< SCL pin connected to the RTC
#define PIN_NUM_LED_RED				GPIO_NUM_21		//!< Output pin connected to the red LED
#define PIN_NUM_LED_GREEN			GPIO_NUM_17		//!< Output pin connected to the green LED
#define PIN_NUM_ON_BOARD_LED		GPIO_NUM_13		//!< Output Pin connected to the red LED next to the USB plug
#define PIN_NUM_BUTTON_1			GPIO_NUM_39		//!< Input pin connected to button 1
#define PIN_NUM_BUTTON_2			GPIO_NUM_36		//!< Input pin connected to button 2
#define PIN_NUM_SD_MISO				GPIO_NUM_19		//!< MISO pin of the SD MMC SPI (HSPI)
#define PIN_NUM_SD_MOSI				GPIO_NUM_18		//!< MOSI pin of the SD MMC SPI (HSPI)
#define PIN_NUM_SD_CLK				GPIO_NUM_5		//!< Clock pin of the SD MMC SPI (HSPI)
#define PIN_NUM_SD_CS				GPIO_NUM_33		//!< Chip select pin of the SD MMC
#define PIN_NUM_SENSOR_CS1			GPIO_NUM_14		//!< Chip select pin of the sensor strip 1
#define PIN_NUM_SENSOR_CS2			GPIO_NUM_32		//!< Chip select pin of the sensor strip 2
#define PIN_NUM_SENSOR_CS3			GPIO_NUM_15		//!< Chip select pin of the sensor strip 3
#define PIN_NUM_SENSOR_CS4			GPIO_NUM_27		//!< Chip select pin of the sensor strip 4
#define PIN_NUM_GAITMONITOR_CS		GPIO_NUM_16		//!< Chip select pin of the gait monitor

/**
 * @brief This type represents all data included in one SocketSense sample.
 */
typedef struct {
	uint64_t		timestamp_usec;		/**< UNIX timestamp in us associated with the start of the data collection for this sample.*/
	bme280_data_t 	bme280_data;		/**< Data recorded from the BME280 (Temperature, Humidity, Atmospheric Pressure). */
	uint16_t 		sensorstrip_data[CONFIG_SOCKETSENSE_SENSOR_COUNT][CONFIG_SOCKETSENSE_SENSEL_COUNT];
	uint32_t		sampling_time;		/**< Time in us it took to record the data */
	uint32_t 		battery_voltage;	/**< Last read battery voltage in mV*/
} SocketSense_Sample_t;

/**
 * @brief The queue that is used to send data to the database task.
 */
QueueHandle_t data_queue;


#endif /* MAIN_INCLUDE_KTHSOCKETSENSE_H_ */
