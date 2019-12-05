/**
 * @file sd_logging.h
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
#ifndef COMPONENTS_SD_LOGGING_H_
#define COMPONENTS_SD_LOGGING_H_

#include <stddef.h>

/**
 * @brief Initialize the SD-Card and mounts the partition.
 *
 * In SPI mode, there needs to be a 10K Ohm resistor between MOSI and VCC (i.e. a pull up).
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t sd_logging_init();

/**
 * @brief Deinitialize the SD-card, unmount partition and disable SDMMC
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t sd_logging_deinit();

/**
 * @brief This function loads the configuration from the file config.txt on the SD-card.
 *
 * The values are only written if all values are found in the file.
 * In addition, this function creates the file for the specified user ID, should it not yet be
 * available on the SD-card.
 *
 * @param wifi_ssid Destination pointer for the WIFI SSID.
 * @param wifi_pw Destination pointer for the WIFI Password.
 * @param uid Destination pointer for the user-id.
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t sd_load_configuration(uint8_t *wifi_ssid, uint8_t *wifi_pw, uint8_t *uid);

/**
 * @brief This function adds the null terminated string str to the log-file.
 *
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t sd_logging_log(char* str);

#endif /* COMPONENTS_SD_LOGGING_H_ */
