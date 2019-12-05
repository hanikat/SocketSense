/**
 * @file pcf8523.c
 * @brief Driver for the PCF8523 RTC IC.
 *
 * These functions provide access to the PCF8523 and allow to set the current ESP time
 * according to the PCF8523 time.
 *
 * @author Matthias Becker
 * @date June 24. 2019
 */
#include <esp_err.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"

#include "KTHSocketSense.h"

static const char *TAG = "PCF8523";

#define ADDRESS_PCF8523 0x68

/**
 * @brief Convert BCD to Integer
 *
 * @param bcd 8 bit BCD value.
 * @return Converted integer value.
 */
static uint8_t bcdToInt(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0f);
}

esp_err_t pcf8523_init(){

	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = PIN_NUM_PCF8523_SDA;
	conf.scl_io_num = PIN_NUM_PCF8523_SCL;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;

	ESP_ERROR_CHECK( i2c_param_config(I2C_NUM_0, &conf) );
	ESP_ERROR_CHECK( i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) );

	ESP_LOGI(TAG, "Initialized");

	return ESP_OK;
}

esp_err_t pcf8523_setRtcTime(){

	esp_err_t ret;
	uint8_t data[7];

	ESP_LOGI(TAG, "Setting the ESP time");

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ADDRESS_PCF8523 << 1) | I2C_MASTER_WRITE, 1);	//switch to write mode
	i2c_master_write_byte(cmd, 0x03 , 1);										//set register to 0x03 (start of the time)
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ADDRESS_PCF8523 << 1) | I2C_MASTER_READ, 1);	//switch to read mode
	i2c_master_read(cmd, data, 7, 0);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if (ret != ESP_OK) {
		return ret;
	}

	struct tm tm;
	tm.tm_year = 100 + bcdToInt(data[6]);
	tm.tm_mon = bcdToInt(data[5]) - 1;
	tm.tm_mday = bcdToInt(data[3]);
	tm.tm_hour = bcdToInt(data[2]);
	tm.tm_min = bcdToInt(data[1]);
	tm.tm_sec = bcdToInt(data[0]);
	time_t t = mktime(&tm);
	ESP_LOGI(TAG, "Setting CET time: %s", asctime(&tm));
	struct timeval now = { .tv_sec = t };
	if(settimeofday(&now, NULL) != 0){
		return ESP_FAIL;
	}

	setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);	//set the timezone (Stockholm), string taken from here https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
	tzset();

	return ESP_OK;
}

void pcf8523_getEspTimeString(uint8_t *buffer){
	time_t now;
	struct tm timeinfo;
	time(&now);

	time(&now);
	localtime_r(&now, &timeinfo);
	strftime((char*)buffer, sizeof(buffer), "%c", &timeinfo);
}

void pcf8523_getEspTimestamp(uint64_t* timestamp){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*timestamp = (1000000 * (uint64_t)tv.tv_sec) + (uint64_t)tv.tv_usec;
}

