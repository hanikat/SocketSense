/**
 * @file data_collector.c
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
#include <stddef.h>
#include <time.h>
#include <sys/time.h>

#include "esp_system.h"
#include "esp_log.h"
#include <esp_err.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "data_collector.h"
#include "error_handler.h"
#include "bme280.h"
#include "socketsense_sensor.h"
#include "gait_monitor.h"
#include "pcf8523.h"
#include "KTHSocketSense.h"

static const char *TAG = "DATA_COLLECTOR";

TaskHandle_t dataCollectionTask;

uint32_t dataCollector_initialized = 0;

/**
 * This function initializes the data collector component.
 */
esp_err_t data_collector_init(void)
{
	esp_err_t retval = ESP_OK;

	spi_device_handle_t spi_bme280;
	spi_device_handle_t spi_socketsense_sensor;
	spi_device_handle_t spi_gait_monitor;

	spi_bus_config_t buscfg={
			.miso_io_num=PIN_NUM_SENSOR_MISO,
	        .mosi_io_num=PIN_NUM_SENSOR_MOSI,
	        .sclk_io_num=PIN_NUM_SENSOR_CLK,
	        .quadwp_io_num=-1,
	        .quadhd_io_num=-1,
	        .max_transfer_sz=320*2+8
	};

	spi_device_interface_config_t dev_bme280_cfg={
		.clock_speed_hz=16*1000*1000,           		//Clock out at 10 MHz
		.mode=0,                                		//SPI mode 0
		.spics_io_num=PIN_NUM_BME280_CS,        		//CS pin used for the BME280
		.queue_size=10,                          		//We want to be able to queue 7 transactions at a time
	};

	spi_device_interface_config_t dev_socketsense_sensor_cfg={
		.clock_speed_hz=16*1000*1000,           			//Clock out at 10 MHz
		.mode=0,                               			//SPI mode 0
		.spics_io_num=-1,  								//CS not used, the different sensor strips are addressed via GPIOs
		.queue_size=10,                          		//We want to be able to queue 7 transactions at a time
	};

	spi_device_interface_config_t dev_gait_monitor_cfg={
		.clock_speed_hz=16*1000*1000,           		//Clock out at 10 MHz
		.mode=3,                                		//SPI mode 3
		.spics_io_num=PIN_NUM_GAITMONITOR_CS,   		//CS pin used for the gait monitor
		.queue_size=10,                          		//We want to be able to queue 7 transactions at a time
	};

	//Initialize the SPI bus
	ESP_ERROR_CHECK( spi_bus_initialize(VSPI_HOST, &buscfg, 2) );

	//Attach the components to the SPI bus (there can be max. 3 components!)
	ESP_ERROR_CHECK( spi_bus_add_device(VSPI_HOST, &dev_bme280_cfg, &spi_bme280) );
	ESP_ERROR_CHECK( spi_bus_add_device(VSPI_HOST, &dev_socketsense_sensor_cfg, &spi_socketsense_sensor) );
	ESP_ERROR_CHECK( spi_bus_add_device(VSPI_HOST, &dev_gait_monitor_cfg, &spi_gait_monitor) );

	//Initialize the components that are configured to be used
#if CONFIG_BME280_SENSOR_ACTIVE == 1
	if(bme280_init(spi_bme280) != ESP_OK){
		error_handler_notify(SOCKET_SENSE_ERROR_BME280_NOT_FOUND);
		retval = ESP_FAIL;
	}
#endif

#if CONFIG_SOCKETSENSE_SENSOR_ACTIVE == 1
	if(socketsense_sensor_init(spi_socketsense_sensor) != ESP_OK){
		retval = ESP_FAIL;
	}
#endif

#if CONFIG_GAIT_SENSOR_ACTIVE == 1
	if(gait_monitor_init(spi_gait_monitor) != ESP_OK){
		retval = ESP_FAIL;
	}
#endif

	data_queue = xQueueCreate(10, sizeof(SocketSense_Sample_t));
	if(data_queue == 0){
		ESP_LOGE(TAG, "failed to create the queue");
	}

	if(retval == ESP_OK){
		ESP_LOGI(TAG, "init");
		dataCollector_initialized = 1;
	}else{
		ESP_LOGE(TAG, "Initialization failed!");
	}

	return retval;
}

/**
 * Main task of the data collector.
 */
void data_collector_task(void * pvParameters)
{
	TickType_t 				xLastWakeTime;
	uint32_t 				ulNotifiedValue;
	SocketSense_Sample_t 	sample;

	struct timeval start;
	struct timeval stop;

	ESP_LOGI(TAG, "task started on core=%i", xPortGetCoreID());

	xLastWakeTime = xTaskGetTickCount();

	while(1){

		if(xTaskNotifyWait( 0x00, ULONG_MAX, &ulNotifiedValue, 0) == pdTRUE){	//check if a notification has been received
			if(ulNotifiedValue == DATA_COLLECTOR_NOTIFY_STOP){
				ESP_LOGI(TAG, "Notification received, suspending...");
				vTaskSuspend(NULL);												//suspend this task
				ESP_LOGI(TAG, "Woke up again...");
				xLastWakeTime = xTaskGetTickCount();							//update the last wake time after resuming
			}
		}

		gettimeofday(&start, NULL);
		pcf8523_getEspTimestamp(&sample.timestamp_usec);						//get the timestamp for the sample

#if CONFIG_BME280_SENSOR_ACTIVE == 1
		bme280_readSensorData(&sample.bme280_data);							//read the BME280 data
#endif

#if CONFIG_SOCKETSENSE_SENSOR_ACTIVE == 1
		socketsense_sensor_readSensorData(&sample.sensorstrip_data);			//read the sensor elements
#endif
		gettimeofday(&stop, NULL);

		sample.sampling_time = (uint32_t)(stop.tv_usec - start.tv_usec);		//collect statistics of the measurement
		sample.battery_voltage = getBatteryVoltage();							//add the last battery voltage value (in mV)

		if(xQueueSend(data_queue, &sample, (TickType_t) 0) != pdTRUE){
			ESP_LOGE(TAG, "Message could not be sent!");
		}else{
			ESP_LOGI(TAG, "Message sent!");
		}


		vTaskDelayUntil( &xLastWakeTime, DATA_COLLECTOR_TASK_PERIOD_MS / portTICK_PERIOD_MS );
	}
}

/**
 * This function is called to start the data collector task
 */
esp_err_t data_collector_start()
{
	if(dataCollector_initialized){
		if(dataCollectionTask == NULL){	//only do this if the task has not been created already
			xTaskCreatePinnedToCore(data_collector_task, "data_collector", 10000, NULL, 1, &dataCollectionTask, DATA_COLLECTOR_CPU);
			vTaskSuspend(dataCollectionTask);	//suspend the task right away
			ESP_LOGI(TAG, "start");
		}
	}
	return ESP_OK;
}

/**
 * This function returns the task handle to the data collector task.
 * This can be used to send notifications to the data collector task
 */
TaskHandle_t data_collector_getTaskHandle(){
	return dataCollectionTask;
}

esp_err_t data_collector_resume(){

	if(dataCollector_initialized > 0){
		vTaskResume(dataCollectionTask);
	}else{
		ESP_LOGE(TAG, "Can't resume task, component not initialized!");
		return ESP_FAIL;
	}

	return ESP_OK;
}

esp_err_t data_collector_stop(){

	if(dataCollector_initialized > 0){
		xTaskNotify(dataCollectionTask, DATA_COLLECTOR_NOTIFY_STOP, eSetValueWithOverwrite);
	}else{
		ESP_LOGE(TAG, "Can't send stop signal, component not initialized!");
		return ESP_FAIL;
	}

	return ESP_OK;
}
