/*
 * error_handler.c
 *
 *  Created on: 15 Jul 2019
 *      Author: matthiasbecker
 */
#include "esp_system.h"
#include <esp_err.h>
#include "esp_log.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "KTHSocketSense.h"
#include "error_handler.h"

static const char *TAG_BATTERY_TASK = "BATTERY_CHECK";
static const char *TAG = "ERROR_HANDLER";

TaskHandle_t battery_handle;
TaskHandle_t error_task_handle;
uint32_t battery_voltage;


void battery_task(void * pvParameters){

	TickType_t xLastWakeTime;
	static esp_adc_cal_characteristics_t *adc_chars;
	uint32_t value = 0;
	uint32_t tmp;
	uint32_t i = 0;

	ESP_LOGI(TAG_BATTERY_TASK, "Battery task started on core=%i", xPortGetCoreID());

	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);

	//Characterize ADC
	adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, ESP_ADC_CAL_VAL_EFUSE_VREF, adc_chars);

	if(val_type != ESP_ADC_CAL_VAL_EFUSE_VREF){
		ESP_LOGI(TAG_BATTERY_TASK, "No EFUSE VREF set for this ESP32! Battery will not be monitored!");
		gpio_set_level(PIN_NUM_ON_BOARD_LED, 1);
	}

	xLastWakeTime = xTaskGetTickCount();

	while(1){
		value = 0;

		for(i = 0; i < BATTERY_NO_OF_SAMPLES; i++){

			tmp = 0;
			 ESP_ERROR_CHECK( esp_adc_cal_get_voltage(ADC1_CHANNEL_7, adc_chars, &tmp) );
			value += tmp;
		}

		value /= BATTERY_NO_OF_SAMPLES;

		uint32_t voltage = value;
		voltage = voltage * 2;
		ESP_LOGI(TAG_BATTERY_TASK, "Voltage: %i mV", voltage);

		battery_voltage = voltage;

		if(voltage < BATTERY_WARNING_VOLTAGE){
			error_handler_notify(SOCKET_SENSE_WARNING_BATTERY_LEVEL_NOT_OK);
		}else{
			error_handler_notify(SOCKET_SENSE_WARNING_BATTERY_LEVEL_OK);
		}

		vTaskDelayUntil( &xLastWakeTime, BATTERY_TASK_PERIOD_MS / portTICK_PERIOD_MS );
	}
}

void error_handler_notify(socketsense_error_t error_code){
	xTaskNotify(error_task_handle, (uint32_t)error_code, eSetBits);
}

void error_handler_task(void * pvParameters){
	socketsense_error_t error_value;

	while(xTaskNotifyWait( 0x00, ULONG_MAX, &error_value, ULONG_MAX) == pdTRUE){	//check if a notification has been received

		/**
		 * First the errors are handled. Errors are non recoverable scenarios, this means the program is
		 * not progressing and we visualize the error by blinking the on-board LED.
		 */
		if((error_value & SOCKET_SENSE_ERROR_BME280_NOT_FOUND) > 0){
			while(1){
				gpio_set_level(PIN_NUM_ON_BOARD_LED, 1);
				vTaskDelay(200 / portTICK_PERIOD_MS);
				gpio_set_level(PIN_NUM_ON_BOARD_LED, 0);
				vTaskDelay(200 / portTICK_PERIOD_MS);
			}
		}
		if((error_value & SOCKET_SENSE_ERROR_02) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_03) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_04) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_05) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_06) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_07) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_08) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_09) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_10) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_11) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_12) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_13) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_14) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_15) > 0){

		}
		if((error_value & SOCKET_SENSE_ERROR_16) > 0){

		}

		/**
		 * These are warning states that can set specific actions but do not halt the program.
		 */
		if((error_value & SOCKET_SENSE_WARNING_BATTERY_LEVEL_NOT_OK) > 0){
			gpio_set_level(PIN_NUM_ON_BOARD_LED, 1);
		}
		if((error_value & SOCKET_SENSE_WARNING_BATTERY_LEVEL_OK) > 0){
			gpio_set_level(PIN_NUM_ON_BOARD_LED, 0);
		}
		if((error_value & SOCKET_SENSE_WARNING_3) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_4) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_5) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_6) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_7) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_8) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_9) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_10) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_11) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_12) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_13) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_14) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_15) > 0){

		}
		if((error_value & SOCKET_SENSE_WARNING_16) > 0){

		}
	}
}

esp_err_t error_handler_init(void){
	gpio_pad_select_gpio(PIN_NUM_ON_BOARD_LED);
	gpio_set_direction(PIN_NUM_ON_BOARD_LED, GPIO_MODE_OUTPUT);			//red on-board LED
	gpio_set_level(PIN_NUM_ON_BOARD_LED, 0);

	xTaskCreatePinnedToCore(battery_task, "battery_check", 10000, NULL, 1, &battery_handle, 0);	//create the battery task to alert if the power gets low
	xTaskCreatePinnedToCore(error_handler_task, "error_handler", 10000, NULL, 1, &error_task_handle, 0);	//create the battery task to alert if the power gets low

	ESP_LOGI(TAG, "init");
	return ESP_OK;
}

uint32_t getBatteryVoltage(){
	return battery_voltage;
}
