/**
 * @file sd_logging.c
 * @brief Component that manages the SD-card.
 *
 * The SD-card is used to store configuration values, such as WIFI SSID and password, as well as patient ID.
 * This allows to avoid rebuilding the project for different patients.
 *
 * In addition, the SD-card is used to log measurement values along with the network transmission.
 *
 * @author Matthias Becker
 * @date June 21. 2019
 */
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <esp_err.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#include "KTHSocketSense.h"

static const char *TAG = "SD_LOGGING";

uint8_t sd_initialized = 0;
sdmmc_card_t* card;
FILE* logFile = NULL;

esp_err_t sd_logging_init()
{
    
	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
	slot_config.gpio_miso = PIN_NUM_SD_MISO;
	slot_config.gpio_mosi = PIN_NUM_SD_MOSI;
	slot_config.gpio_sck  = PIN_NUM_SD_CLK;
	slot_config.gpio_cs   = PIN_NUM_SD_CS;
	// This initializes the slot without card detect (CD) and write protect (WP) signals.
	// Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.

	// Options for mounting the filesystem.
	// If format_if_mount_failed is set to true, SD card will be partitioned and
	// formatted in case when mounting fails.
	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
			.format_if_mount_failed = false,
			.max_files = 5,
			.allocation_unit_size = 16 * 1024
	};

	// Use settings defined above to initialize SD card and mount FAT filesystem.
	// Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
	// Please check its source code and implement error recovery when developing
	// production applications.
	esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount filesystem. "
					"If you want the card to be formatted, set format_if_mount_failed = true.");
		} else {
			ESP_LOGE(TAG, "Failed to initialize the card (%s). Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
		}
		return ESP_FAIL;
	}

	// Card has been initialized, print its properties
	sdmmc_card_print_info(stdout, card);

	ESP_LOGI(TAG, "SD-Logging Initialized");

	sd_initialized = 1;

	return ESP_OK;
}

esp_err_t sd_logging_deinit(){
	esp_err_t ret = esp_vfs_fat_sdmmc_unmount();

	return ret;
}

esp_err_t sd_load_configuration(uint8_t *wifi_ssid, uint8_t *wifi_pw, uint8_t *uid){

	uint8_t tmp_wifi_ssid[20];
	uint8_t tmp_wifi_pw[20];
	uint8_t tmp_uid[8];
	uint8_t uid_filename[20];

	memset(&uid_filename, 0, sizeof(uid_filename));
	strcat((char*) uid_filename, "/sdcard/");

	if(sd_initialized != 1){
		ESP_LOGE(TAG, "Configuration not loaded, SD-card not initialized/present.");
		return ESP_FAIL;
	}

	struct stat st;
	if (stat("/sdcard/config.txt", &st) == 0) {					//check if the file exists
		FILE* f = fopen("/sdcard/config.txt", "r");				//open the file for reading
		if (f == NULL) {
			ESP_LOGE(TAG, "Failed to open file for reading");
			return ESP_FAIL;
		}

		char line[64];

		for(int i = 0; i < 3; i++){

			if(fgets(line, sizeof(line), f) == NULL){
				ESP_LOGE(TAG, "Only the first %i configuration values provided in config.txt!", i);
				return ESP_FAIL;
			}
			char* pos = strchr(line, '\n');
			if (pos) {
				*pos = '\0';
			}
			ESP_LOGI(TAG, "Read configuration value %i '%s' from config.txt", i, line);

			if(i == 0) memcpy(tmp_wifi_ssid, line, pos - line + 1);
			if(i == 1) memcpy(tmp_wifi_pw, line, pos - line + 1);
			if(i == 2) memcpy(tmp_uid, line, pos - line + 1);
		}

		fclose(f);

		memcpy(wifi_ssid, tmp_wifi_ssid, strlen((char*)tmp_wifi_ssid));
		memcpy(wifi_pw, tmp_wifi_pw, strlen((char*)tmp_wifi_pw));
		memcpy(uid, tmp_uid, strlen((char*)tmp_uid));

		ESP_LOGI(TAG, "Configuration successfully loaded from config.txt.");


		strcat((char*) uid_filename, (char*)tmp_uid);
		strcat((char*) uid_filename, ".txt");
		ESP_LOGI(TAG, "Searching log-file %s for user ID %s", uid_filename, tmp_uid);

		if (stat((char*)uid_filename, &st) != 0) {				//check if the log-file for the defined user ID does not yet exist
			ESP_LOGI(TAG, "Did not find the file: %s", uid_filename);
		}else{
			ESP_LOGI(TAG, "Found the file: %s", uid_filename);
		}

		logFile = fopen((char*)uid_filename, "a");
		if (logFile == NULL) {
			ESP_LOGE(TAG, "Failed to open file %s for append access", uid_filename);
			return ESP_FAIL;
		}

	}else{
		ESP_LOGI(TAG, "The configuration file does not exist.");
		return ESP_FAIL;
	}

	return ESP_OK;
}

esp_err_t sd_logging_log(char* str){
	if(sd_initialized != 1){
		ESP_LOGI(TAG, "Configuration not loaded, SD-card not initialized/present.");
		return ESP_FAIL;
	}

	if(logFile == NULL){
		ESP_LOGI(TAG, "Log-file has not been setup.");
		return ESP_FAIL;
	}

	char* pos = strchr(str, '\0');				//adding a newline
	if (pos) {
		*pos = '\n';
	}
	fprintf(logFile, str, card->cid.name);
	fflush(logFile);
	fsync(fileno(logFile));

	return ESP_OK;
}
