/**
 * @file gait_monitor.c
 * @brief Component that receives measurement data from the gait monitor.
 *
 * The gait monitor can be used to query the current gait cycle that can then be added to the measured data.
 *
 * @author Matthias Becker
 * @date June 12. 2019
 */
#include <stddef.h>

#include <esp_err.h>
#include "esp_system.h"
#include "esp_log.h"

#include "gait_monitor.h"

static const char *TAG = "GAIT MONITOR";

spi_device_handle_t spiHandle_gaitMonitor;

esp_err_t gait_monitor_init(spi_device_handle_t _spi)
{
	spiHandle_gaitMonitor = _spi;

	ESP_LOGI(TAG, "Initialized");
    
	return ESP_OK;
}
