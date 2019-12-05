/**
 * @file pcf8523.h
 * @brief Driver for the PCF8523 RTC IC.
 *
 * These functions provide access to the PCF8523 and allow to set the current ESP time
 * according to the PCF8523 time.
 *
 * @author Matthias Becker
 * @date June 24. 2019
 */
#ifndef COMPONENTS_PCF8523_H_
#define COMPONENTS_PCF8523_H_

#include <stddef.h>

/**
 * @brief Initialize PCF8523.
 *
 * This function configures the I2C bus to communicate with the PCD8523.
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t pcf8523_init();

/**
 * @brief Set ESP time according to current PCF8523 time.
 *
 * This function reads the current RTC time from the PCF8523
 * over I2C and sets the internal ESP time accordingly.
 * This needs to be performed only once at startup.
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t pcf8523_setRtcTime();

/**
 * @brief Get formatted time string.
 *
 * This function writes the current time string to the provided buffer.
 * Note that this is the ESPs local time, not the time read from the PCF8523.
 * @param buffer Pointer to the buffer for the time string.
 */
void pcf8523_getEspTimeString(uint8_t *buffer);

/**
 * @brief Get POSIX timestamp in us.
 *
 * This function returns the POSIX time in us
 * @param timestamp POSIX time in us.
 */
void pcf8523_getEspTimestamp(uint64_t* timestamp);

#endif /* COMPONENTS_PCF8523_H_ */
