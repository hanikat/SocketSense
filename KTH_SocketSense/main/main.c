
/*
 * main.c
 *
 *  Created on: 12 Jun 2019
 *  Author: matthiasbecker
 *
 */
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "driver/gpio.h"

#include "error_handler.h"
#include "data_collector.h"
#include "influxdb.h"
#include "sd_logging.h"
#include "pcf8523.h"
#include "KTHSocketSense.h"

#define ESP_INTR_FLAG_DEFAULT 	0

uint8_t wifi_active = 0;

static const char *TAG = "APP_MAIN";

uint8_t tmp_lvl = 0;

/**
 * Event Handler for ESP events
 * @param ctx
 * @param event
 * @return
 */
esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch(event->event_id) {
		case SYSTEM_EVENT_STA_GOT_IP:
			wifi_active = 1;
			gpio_set_level(PIN_NUM_LED_GREEN, 1);						//turn on the green LED to signal that an IP has been assigned
			influxdb_enable();											//once the IP has been assigned the remote database can be accessed
			data_collector_start();										//now that the wifi connection is setup we can start the data collection

			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			wifi_active = 0;											//start blinking the green LED if the IP was lost
			ESP_ERROR_CHECK( esp_wifi_connect() );
			break;
		default:
			break;
	}
    return ESP_OK;
}

void app_main(void)
{
	int led_level = 0;													//current level of the LED GPIOs
	int measuring = 0;
	int button_state = 0;

	uint8_t uid[8];
	memset(&uid, 0, sizeof(uid));

	error_handler_init();												//start the error handler task (this includes the battery level check)

	//Configure GPIO for the external LEDs
	gpio_pad_select_gpio(PIN_NUM_LED_RED);
	gpio_pad_select_gpio(PIN_NUM_LED_GREEN);

	gpio_set_direction(PIN_NUM_LED_RED, GPIO_MODE_OUTPUT);				//red
	gpio_set_direction(PIN_NUM_LED_GREEN, GPIO_MODE_OUTPUT);			//green

	gpio_set_direction(PIN_NUM_BUTTON_1, GPIO_MODE_INPUT);
	gpio_set_direction(PIN_NUM_BUTTON_2, GPIO_MODE_INPUT);

	gpio_set_level(PIN_NUM_LED_RED, 0);									//Turn off the red and green LED
	gpio_set_level(PIN_NUM_LED_GREEN, 0);

	if(sd_logging_init() != ESP_OK){									//Initialize the SD-card
		ESP_LOGI(TAG, "No SD-Card connected!");
	}

	ESP_ERROR_CHECK( pcf8523_init() );									//initialize the I2C driver
	ESP_ERROR_CHECK( pcf8523_setRtcTime() );							//Read the current time from the RTC and configure ESP time accordingly

    nvs_flash_init();													//initialize the external flash memory
    tcpip_adapter_init();												//creates the LwIP core task and does the LwIP initialization
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );		//register a handler to the ESP event loop
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();				//initializes the wifi configuration values to defaults
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );								//create the wifi driver task and initialize the driver
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .bssid_set = false
        }
    };

    sd_load_configuration(sta_config.sta.ssid, sta_config.sta.password, uid);	//try to load the configuration from the SD-card

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

	data_collector_init();												//initialize the data collection component, this does not start a task yet!
	influxdb_init(uid);													//initialize the influxDB component, this does not start the task yet!

    while (true) {														//from here on all happens in the created tasks and this part only toggles the blue LED
        if(wifi_active == 0){
        	gpio_set_level(PIN_NUM_LED_GREEN, led_level);
        	led_level = !led_level;
        }

        if(gpio_get_level(PIN_NUM_BUTTON_1) == 0 && button_state == 1){

        	if(measuring == 1){
        		ESP_LOGI(TAG, "Sending notification to data collector task...");
        		if(data_collector_stop() == ESP_OK){
        			gpio_set_level(PIN_NUM_LED_RED, 0);	//move this to the data collector task
        			measuring = 0;
        		}
        	}else{
        		ESP_LOGI(TAG, "Resuming data collector task...");
        		if(data_collector_resume() == ESP_OK){
        			gpio_set_level(PIN_NUM_LED_RED, 1);	//move this to the data collector task
        			measuring = 1;
        		}
        	}
        }
        button_state = gpio_get_level(PIN_NUM_BUTTON_1);
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

