/*
 * bme280.c
 *
 *  Created on: 12 Jun 2019
 *  Author: matthiasbecker
 *
 *  Ported from here: https://github.com/roamingthings/esp32-bme280/tree/master/components
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "esp_system.h"
#include "esp_log.h"
#include <esp_err.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bme280.h"

static const char *TAG = "BME_280";

/**
 * The handle to the SPI interface that is used.
 */
spi_device_handle_t spi;

/**
 * Calibration data that is read from the sensors non-colatile memory
 */
bme280_calib_data_t _bme280_calib;

/**
 * Used to compute the compensation values.
 */
int32_t _t_fine;

/*****Private Functions Definitions*************************************************/

uint8_t readRegister8(uint8_t reg);
uint16_t read16BitBigEndianRegister(uint8_t reg);
uint16_t read16BitLittleEndianRegister(uint8_t register_address);
int16_t read16BitSignedLittleEndianRegister(uint8_t register_address);
uint32_t readRegister24(uint8_t reg);
esp_err_t writeRegister8(uint8_t register_address, uint8_t data);
bme280_adc_data_t burstReadMeasurement();
BME280_S32_t compensate_T(BME280_S32_t adc_T);
BME280_U32_t compensate_P(BME280_S32_t adc_P);
BME280_U32_t compensate_H(BME280_S32_t adc_H);
float convertUncompensatedTemperature(int32_t adc_T);
float convertUncompensatedPressure(BME280_S32_t adc_P);
float convertUncompensatedHumidity(BME280_S32_t adc_H);
void readCoefficients(void);

/*****Public Functions**************************************************************/

/**
 * Initialize the component and the sensor
 */
esp_err_t bme280_init(spi_device_handle_t _spi)
{
	spi = _spi;

	//get the chip ID of the BME280 sensor
	uint8_t chipId = bme280_readChipId();

	if(chipId != 0x60){
		ESP_LOGE(TAG, "BME Sensor not detected on SPI bus! (read: 0x%.2x)", chipId);
		return ESP_FAIL;
	}

	ESP_LOGI(TAG, "BME280 Sensor detected on SPI bus!");

	vTaskDelay(1000/portTICK_PERIOD_MS);

	readCoefficients();

	writeRegister8(BME280_REGISTER_CONTROLHUMID, 0x01); // 16x oversampling
	writeRegister8(BME280_REGISTER_CONTROL, 0xB7);      // 16x oversampling, normal mode

	return ESP_OK;
}

/**
 * Read the current data values from the sensor
 */
void bme280_readSensorData(bme280_data_t *reading_data){

	bme280_adc_data_t adc_data = burstReadMeasurement();

	reading_data->temperature = convertUncompensatedTemperature(adc_data.adc_data.adc_T);
	reading_data->pressure = convertUncompensatedPressure(adc_data.adc_data.adc_P);
	reading_data->humidity = convertUncompensatedHumidity(adc_data.adc_data.adc_H);

	return;
}

/**
 * Calculates the altitude (in meters) from the specified atmospheric
 * pressure (in hPa), and sea-level pressure (in hPa).
 */
float bme280_altitudeOfPressure(float pressure, float seaLevel){
	// Equation taken from BMP180 datasheet (page 16):
	//  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

	// Note that using the equation from wikipedia can give bad results
	// at high altitude.  See this thread for more information:
	//  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

	float atmospheric = pressure / 100.0F;
	return 44330.0 * (1.0 - pow((double) atmospheric / seaLevel, (double) 0.1903));
}

/**
 * Calculates the pressure at sea level (in hPa) from the specified altitude
 * (in meters), and atmospheric pressure (in hPa).
 */
float bme280_seaLevelForAltitude(float altitude, float atmospheric){
	// Equation taken from BMP180 datasheet (page 17):
	//  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

	// Note that using the equation from wikipedia can give bad results
	// at high altitude.  See this thread for more information:
	//  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

	return atmospheric / pow(1.0 - (altitude / 44330.0), 5.255);
}

/**
 * Read the chip id
 */
uint8_t bme280_readChipId(void){
	uint8_t chip_id = readRegister8(BME280_REGISTER_CHIPID);

	return chip_id;
}

/*****Private Functions*************************************************************/

/**
 * Read an 8-bit value from the sensor.
 */
uint8_t readRegister8(uint8_t reg){

	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length=8*2;
	t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
	t.user = (void*)1;
	t.tx_data[0]=reg | 0x80;	//address and bit 7 high to signal read command
	t.rxlength = 2 * 8;

	esp_err_t ret = spi_device_polling_transmit(spi, &t);
	assert( ret == ESP_OK );

	return t.rx_data[1];
}

/**
 * Read a 16-bit unsigned big endian value from the sensor.
 */
uint16_t read16BitBigEndianRegister(uint8_t reg){

	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length=8*3;
	t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
	t.user = (void*)1;
	t.tx_data[0] = reg | 0x80;	//address and bit 7 high to signal read command
	t.rxlength = 3 * 8;

	esp_err_t ret = spi_device_polling_transmit(spi, &t);
	assert( ret == ESP_OK );

	return (uint16_t)((t.rx_data[1] << 8) | t.rx_data[2]);
}

/**
 * Read a 16-bit unsigned little endian value from the sensor.
 */
uint16_t read16BitLittleEndianRegister(uint8_t register_address){
	uint16_t temp = read16BitBigEndianRegister(register_address);

	return (temp >> 8) | (temp << 8);
}

/**
 * Read a 16-bit signed little endian value from the sensor.
 */
int16_t read16BitSignedLittleEndianRegister(uint8_t register_address){
	return (int16_t) read16BitLittleEndianRegister(register_address);
}

/**
 * Read a 24-bit value from the sensor.
 */
uint32_t readRegister24(uint8_t reg){

	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length=8*4;
	t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
	t.user = (void*)1;
	t.tx_data[0] = reg | 0x80;	//address and bit 7 high to signal read command
	t.rxlength = 4 * 8;

	esp_err_t ret = spi_device_polling_transmit(spi, &t);
	assert( ret == ESP_OK );

	return (uint32_t)((t.rx_data[1] << 16) | (t.rx_data[2] << 8) | t.rx_data[3]);
}

/**
 * Write to a 8-bit data register of the sensor.
 */
esp_err_t writeRegister8(uint8_t register_address, uint8_t data){

	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length=8*2;
	t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
	t.user = (void*)1;
	t.tx_data[0] = register_address & 0x7f;	//address and bit 7 low to signal write command
	t.tx_data[1] = data;
	t.rxlength = 2 * 8;

	esp_err_t ret = spi_device_polling_transmit(spi, &t);
	assert( ret == ESP_OK );

	return ESP_OK;
}

/**
 * Burst read all sensor data. Burst read is highly recommended to make sure that the 3 sensor values
 * are from the same measurement cycle. Additionally it is faster to do it like this.
 */
bme280_adc_data_t burstReadMeasurement(){
	bme280_adc_data_t adc_data;
	uint8_t rx_buffer[9];
	uint8_t tx_buffer[9];

	memset(&tx_buffer, 0, sizeof(tx_buffer));

	tx_buffer[0] = BME280_REGISTER_PRESSUREDATA;

	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length=8*9;
	t.flags = 0;
	t.user = (void*)1;
	t.rxlength = 8 * 9;
	t.rx_buffer = rx_buffer;
	t.tx_buffer = tx_buffer;

	esp_err_t ret = spi_device_polling_transmit(spi, &t);
	assert( ret == ESP_OK );

	adc_data.buffer.pressure.xmsb = 0;
	adc_data.buffer.pressure.msb = rx_buffer[1];
	adc_data.buffer.pressure.lsb = rx_buffer[2];
	adc_data.buffer.pressure.xlsb = rx_buffer[3];

	adc_data.buffer.temperature.xmsb = 0;
	adc_data.buffer.temperature.msb = rx_buffer[4];
	adc_data.buffer.temperature.lsb = rx_buffer[5];
	adc_data.buffer.temperature.xlsb = rx_buffer[6];

	adc_data.buffer.humidity.xmsb = 0;
	adc_data.buffer.humidity.msb = 0;
	adc_data.buffer.humidity.lsb = rx_buffer[7];
	adc_data.buffer.humidity.xlsb = rx_buffer[8];

	return adc_data;
}

/**
 * Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
 * t_fine carries fine temperature as global value
 */
BME280_S32_t compensate_T(BME280_S32_t adc_T){
	BME280_S32_t var1, var2, T;

	var1 = ((((adc_T>>3) - ((BME280_S32_t)_bme280_calib.dig_T1<<1))) * ((BME280_S32_t)_bme280_calib.dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BME280_S32_t)_bme280_calib.dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)_bme280_calib.dig_T1))) >> 12) *
			((BME280_S32_t)_bme280_calib.dig_T3)) >> 14;

	 _t_fine = var1 + var2;

	T = (_t_fine * 5 + 128) >> 8;

	return T;
}

/**
 * Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
 * Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
 */
BME280_U32_t compensate_P(BME280_S32_t adc_P){
	BME280_S64_t var1, var2, p;

	var1 = ((BME280_S64_t)_t_fine) - 128000;
	var2 = var1 * var1 * (BME280_S64_t)_bme280_calib.dig_P6;
	var2 = var2 + ((var1*(BME280_S64_t)_bme280_calib.dig_P5) << 17);
	var2 = var2 + (((BME280_S64_t)_bme280_calib.dig_P4) << 35);
	var1 = ((var1 * var1 * (BME280_S64_t)_bme280_calib.dig_P3) >> 8) + ((var1 * (BME280_S64_t)_bme280_calib.dig_P2) << 12);
	var1 = (((((BME280_S64_t)1) << 47) + var1)) * ((BME280_S64_t)_bme280_calib.dig_P1 ) >> 33;

	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}

	p = 1048576 - adc_P;
	p = (((p<<31) - var2) * 3125) / var1;
	var1 = (((BME280_S64_t)_bme280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((BME280_S64_t)_bme280_calib.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)_bme280_calib.dig_P7) << 4);

	return (BME280_U32_t)p;
}

/**
 * Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
 * Output value of “47445” represents 47445/1024 = 46.333 %RH
 */
BME280_U32_t compensate_H(BME280_S32_t adc_H){
	BME280_S32_t v_x1_u32r;

	v_x1_u32r = (_t_fine - ((BME280_S32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)_bme280_calib.dig_H4) << 20) - (((BME280_S32_t)_bme280_calib.dig_H5) * v_x1_u32r)) +
			((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)_bme280_calib.dig_H6)) >> 10) * (((v_x1_u32r *
					((BME280_S32_t)_bme280_calib.dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
					((BME280_S32_t)_bme280_calib.dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)_bme280_calib.dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

	return (BME280_U32_t)(v_x1_u32r >> 12);
}

float convertUncompensatedTemperature(int32_t adc_T){
	adc_T >>= 4;
	BME280_S32_t T = compensate_T(adc_T);

	return (float) T / 100.0;
}

float convertUncompensatedPressure(BME280_S32_t adc_P){
	adc_P >>= 4;
	BME280_U32_t p = compensate_P(adc_P);

	return (float) p / 256;
}

float convertUncompensatedHumidity(BME280_S32_t adc_H){
	BME280_U32_t h = compensate_H(adc_H);

	return h / 1024.0;
}

/**
 * Read the coefficients from the sensors non-volatile memory. These coefficients are needed to
 * compute the correct values of the measured data.
 */
void readCoefficients(void){
	_bme280_calib.dig_T1 = (uint16_t)read16BitLittleEndianRegister(BME280_REGISTER_DIG_T1);
	_bme280_calib.dig_T2 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_T2);
	_bme280_calib.dig_T3 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_T3);

	ESP_LOGD(TAG, "T1: %u, T2: %d, T3: %d", _bme280_calib.dig_T1, _bme280_calib.dig_T2, _bme280_calib.dig_T3);


	_bme280_calib.dig_P1 = (uint16_t)read16BitLittleEndianRegister(BME280_REGISTER_DIG_P1);
	_bme280_calib.dig_P2 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_P2);
	_bme280_calib.dig_P3 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_P3);
	_bme280_calib.dig_P4 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_P4);
	_bme280_calib.dig_P5 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_P5);
	_bme280_calib.dig_P6 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_P6);
	_bme280_calib.dig_P7 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_P7);
	_bme280_calib.dig_P8 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_P8);
	_bme280_calib.dig_P9 = (int16_t)read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_P9);

	ESP_LOGD(TAG, "P1: %u, P2: %d, P3: %d, P4: %d, P5: %d, P6: %d, P7: %d, P8: %d, P9: %d",
			_bme280_calib.dig_P1,
			_bme280_calib.dig_P2,
			_bme280_calib.dig_P3,
			_bme280_calib.dig_P4,
			_bme280_calib.dig_P5,
			_bme280_calib.dig_P6,
			_bme280_calib.dig_P7,
			_bme280_calib.dig_P8,
			_bme280_calib.dig_P9);

	_bme280_calib.dig_H1 = readRegister8(BME280_REGISTER_DIG_H1);
	_bme280_calib.dig_H2 = read16BitSignedLittleEndianRegister(BME280_REGISTER_DIG_H2);
	_bme280_calib.dig_H3 = readRegister8(BME280_REGISTER_DIG_H3);
	_bme280_calib.dig_H4 = (readRegister8(BME280_REGISTER_DIG_H4) << 4) |
			(readRegister8(BME280_REGISTER_DIG_H4 + 1) & 0xF);
	_bme280_calib.dig_H5 = (readRegister8(BME280_REGISTER_DIG_H5 + 1) << 4) |
			(readRegister8(BME280_REGISTER_DIG_H5) >> 4);
	_bme280_calib.dig_H6 = (int8_t) readRegister8(BME280_REGISTER_DIG_H6);
}
