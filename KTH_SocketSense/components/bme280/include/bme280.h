/**
 * @file bme280.h
 * @brief Driver for the BME280 Environment Sensor.
 *
 * These functions provide access to the BME280 and allow to read temperature,
 * humidity and atmospheric pressure.
 *
 * Ported from here: https://github.com/roamingthings/esp32-bme280/tree/master/components
 *
 * @author Matthias Becker
 * @date June 12. 2019
 */
#ifndef COMPONENTS_BME280_H_
#define COMPONENTS_BME280_H_

#include "driver/spi_master.h"

/**
 * @brief This enum contains all register addresses of the BME280.
 */
enum
{
    BME280_REGISTER_DIG_T1 = 0x88,      //!< BME280_REGISTER_DIG_T1
    BME280_REGISTER_DIG_T2 = 0x8A,      //!< BME280_REGISTER_DIG_T2
    BME280_REGISTER_DIG_T3 = 0x8C,      //!< BME280_REGISTER_DIG_T3

    BME280_REGISTER_DIG_P1 = 0x8E,      //!< BME280_REGISTER_DIG_P1
    BME280_REGISTER_DIG_P2 = 0x90,      //!< BME280_REGISTER_DIG_P2
    BME280_REGISTER_DIG_P3 = 0x92,      //!< BME280_REGISTER_DIG_P3
    BME280_REGISTER_DIG_P4 = 0x94,      //!< BME280_REGISTER_DIG_P4
    BME280_REGISTER_DIG_P5 = 0x96,      //!< BME280_REGISTER_DIG_P5
    BME280_REGISTER_DIG_P6 = 0x98,      //!< BME280_REGISTER_DIG_P6
    BME280_REGISTER_DIG_P7 = 0x9A,      //!< BME280_REGISTER_DIG_P7
    BME280_REGISTER_DIG_P8 = 0x9C,      //!< BME280_REGISTER_DIG_P8
    BME280_REGISTER_DIG_P9 = 0x9E,      //!< BME280_REGISTER_DIG_P9

    BME280_REGISTER_DIG_H1 = 0xA1,      //!< BME280_REGISTER_DIG_H1
    BME280_REGISTER_DIG_H2 = 0xE1,      //!< BME280_REGISTER_DIG_H2
    BME280_REGISTER_DIG_H3 = 0xE3,      //!< BME280_REGISTER_DIG_H3
    BME280_REGISTER_DIG_H4 = 0xE4,      //!< BME280_REGISTER_DIG_H4
    BME280_REGISTER_DIG_H5 = 0xE5,      //!< BME280_REGISTER_DIG_H5
    BME280_REGISTER_DIG_H6 = 0xE7,      //!< BME280_REGISTER_DIG_H6

    BME280_REGISTER_CHIPID = 0xD0,      //!< BME280_REGISTER_CHIPID
    BME280_REGISTER_VERSION = 0xD1,     //!< BME280_REGISTER_VERSION
    BME280_REGISTER_SOFTRESET = 0xE0,   //!< BME280_REGISTER_SOFTRESET

    BME280_REGISTER_CAL26 = 0xE1, // R calibration stored in 0xE1-0xF0

    BME280_REGISTER_CONTROLHUMID = 0xF2,//!< BME280_REGISTER_CONTROLHUMID
    BME280_REGISTER_STATUS = 0xF3,      //!< BME280_REGISTER_STATUS
    BME280_REGISTER_CONTROL = 0xF4,     //!< BME280_REGISTER_CONTROL
    BME280_REGISTER_CONFIG = 0xF5,      //!< BME280_REGISTER_CONFIG
    BME280_REGISTER_PRESSUREDATA = 0xF7,//!< BME280_REGISTER_PRESSUREDATA
    BME280_REGISTER_TEMPDATA = 0xFA,    //!< BME280_REGISTER_TEMPDATA
    BME280_REGISTER_HUMIDDATA = 0xFD,   //!< BME280_REGISTER_HUMIDDATA
};

/**
 * @brief Data structure that holds the calibration data of the sensor.
 *
 * This data initially stored on the BME280s non-volatile memory and read
 * during initialization of the component.
 */
typedef struct
{
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;

    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
} bme280_calib_data_t;


/**
 * @brief BME280 32 bit unsigned value
 */
typedef uint32_t BME280_U32_t;

/**
 * @brief BME280 32 bit signed value
 */
typedef int32_t BME280_S32_t;

/**
 * @brief BME280 64 bit unsigned value
 */
typedef int64_t BME280_S64_t;

/**
 * @brief Data structure that holds all parameters that are measured by the BME280.
 */
typedef struct
{
    float temperature;	/**< Temperature.*/
    float humidity;		/**< Humidity.*/
    float pressure;		/**< Pressure.*/
} bme280_data_t;

/**
 * @brief Union to store the raw values of the sensor.
 */
typedef union
{
    struct {
        BME280_S32_t adc_P;
        BME280_S32_t adc_T;
        BME280_S32_t adc_H;
    } adc_data;
    struct {
        struct {
            uint8_t xlsb;
            uint8_t lsb;
            uint8_t msb;
            uint8_t xmsb;
        } pressure;
        struct {
            uint8_t xlsb;
            uint8_t lsb;
            uint8_t msb;
            uint8_t xmsb;
        } temperature;
        struct {
            uint8_t xlsb;
            uint8_t lsb;
            uint8_t msb;
            uint8_t xmsb;
        } humidity;
    } buffer;
} bme280_adc_data_t;

/**
 * @brief Initialize the component and the sensor
 *
 * @param _spi Pointer to the SPI device handle.
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t bme280_init(spi_device_handle_t _spi);

/**
 * @brief Read the current data values from the sensor
 *
 * @param reading_data Pointer to the data structure that should be read to.
 */
void bme280_readSensorData(bme280_data_t *reading_data);

/**
 * @brief Compute the altitude in meters based on the recorded pressure.
 *
 * Calculates the altitude (in meters) from the specified atmospheric
 * pressure (in hPa), and sea-level pressure (in hPa).
 *
 * @param pressure Pressure as measured by the BME280 (hPa).
 * @param seaLevel Pressure at sea level (hPa).
 * @return Altitude in meters.
 */
float bme280_altitudeOfPressure(float pressure, float seaLevel);

/**
 * @brief Calculate the pressure at sea level based on the specified altitude.
 *
 * Calculates the pressure at sea level (in hPa) from the specified altitude
 * (in meters), and atmospheric pressure (in hPa).
 *
 * @param altitude Altitude in meters.
 * @param atmospheric Atmospheric pressure as measured by BME280 (hPa).
 * @return Pressure at sea level (hPa).
 */
float bme280_seaLevelForAltitude(float altitude, float atmospheric);

/**
 * @brief Read the chip ID.
 *
 *The chip ID on the BME280 is always 0x60.
 *
 * @return Chip ID.
 */
uint8_t bme280_readChipId(void);

#endif /* COMPONENTS_BME280_H_ */
