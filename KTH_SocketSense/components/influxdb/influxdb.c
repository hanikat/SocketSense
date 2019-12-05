/*
 * influxdb.c
 *
 *  Created on: 12 Jun 2019
 *  Author: matthiasbecker
 *
 */
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "esp_system.h"
#include "esp_log.h"
#include <esp_err.h>
#include "esp_http_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "sd_logging.h"
#include "KTHSocketSense.h"

static const char *TAG = "INFLUX_DB";

esp_http_client_config_t config;
esp_http_client_handle_t client;

TaskHandle_t influxdb_handle = NULL;

char buffer[200];
uint8_t* user_id;

#define INFLUXDB_CPU 0
#define INFLUXDB_TASK_PERIOD_MS 500

/**
 * This function posts the measurement data to the database.
 * Additionally, the same data is written to the log-file on the SD-card (if available).
 */
void influxdb_post_data(SocketSense_Sample_t _sample);

/**
 * @brief		HTTP event handler function
 *
 * @param[in]	evt	The event to handle
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                // printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

/**
 * This function posts the measurement data to the database.
 * Additionally, the same data is written to the log-file on the SD-card (if available).
 */
void influxdb_post_data(SocketSense_Sample_t _sample){
		esp_err_t err;

		memset(&buffer, 0, sizeof(buffer));	//resetting the buffer
		sprintf(buffer, "socket_data temp=%.2f,hum=%.2f,pres=%.2f,st=%u,bl=%u %llu", _sample.bme280_data.temperature, _sample.bme280_data.humidity, _sample.bme280_data.pressure, _sample.sampling_time, _sample.battery_voltage, _sample.timestamp_usec);

		client = esp_http_client_init(&config);
		esp_http_client_set_method(client, HTTP_METHOD_POST);

		esp_http_client_set_post_field(client, buffer, strlen(buffer));
		err = esp_http_client_perform(client);
		if (err == ESP_OK) {
			ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
					esp_http_client_get_status_code(client),
					esp_http_client_get_content_length(client));
		} else {
			ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
		}

		esp_http_client_cleanup(client);

		sd_logging_log(buffer);
}

/**
 * This function configures all internal values
 */
esp_err_t influxdb_init(uint8_t* uid)
{

	config.host = CONFIG_INFLUXDB_IP;
	config.port = CONFIG_INFLUXDB_PORT;
	config.path = "/write?db=esp32_tst&precision=u";
	config.username = CONFIG_INFLUXDB_USERNAME;
	config.password = CONFIG_INFLUXDB_PASSWORD;
	config.event_handler = _http_event_handler;

	ESP_LOGI(TAG, "init");

	return ESP_OK;
}

void influxdb_task(void * pvParameters){

	TickType_t xLastWakeTime;
	SocketSense_Sample_t data;

	ESP_LOGI(TAG, "task started on core=%i", xPortGetCoreID());

	xLastWakeTime = xTaskGetTickCount();

	while(1){

		while(xQueueReceive(data_queue, &data, 0) == pdTRUE){
#if CONFIG_BME280_SENSOR_ACTIVE == 1
			ESP_LOGI(TAG, "Temperature: %.2foC, Humidity: %.2f%%, Pressure: %.2fPa",
							(double) data.bme280_data.temperature,
							(double) data.bme280_data.humidity,
							(double) data.bme280_data.pressure);
#endif
#if CONFIG_SOCKETSENSE_SENSOR_ACTIVE == 1
			for(int i = 0; i < CONFIG_SOCKETSENSE_SENSOR_COUNT; i++){
				ESP_LOGI(TAG, "SensorStrip-%.2i: SE1: %.4i SE2: %.4i SE3: %.4i SE4: %.4i SE5: %.4i SE6: %.4i SE7: %.4i SE8: %.4i", i,
							data.sensorstrip_data[i][0],
							data.sensorstrip_data[i][1],
							data.sensorstrip_data[i][2],
							data.sensorstrip_data[i][3],
							data.sensorstrip_data[i][4],
							data.sensorstrip_data[i][5],
							data.sensorstrip_data[i][6],
							data.sensorstrip_data[i][7]);
			}
#endif
			ESP_LOGI(TAG, "Sample Time: %u usec", data.sampling_time);
			ESP_LOGI(TAG, "Battery Voltage: %u mV", data.battery_voltage);
			influxdb_post_data(data);
		}

		vTaskDelayUntil( &xLastWakeTime, INFLUXDB_TASK_PERIOD_MS / portTICK_PERIOD_MS );
	}
}

esp_err_t influxdb_enable()
{

	if(influxdb_handle == NULL){	//only do this if the task was not already created
		ESP_LOGI(TAG, "enabled");
		xTaskCreatePinnedToCore(influxdb_task, "influxdb", 10000, NULL, 1, &influxdb_handle, INFLUXDB_CPU);
	}

	return ESP_OK;
}

esp_err_t influxdb_disable()
{
	ESP_LOGI(TAG, "disabled");

	vTaskDelete(influxdb_handle);

	return ESP_OK;
}

esp_err_t influxdb_deinit()
{

	influxdb_disable();
	ESP_LOGI(TAG, "deinit");

	return ESP_OK;
}
