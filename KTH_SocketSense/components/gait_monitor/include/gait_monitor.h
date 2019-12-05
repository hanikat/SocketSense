/**
 * @file gait_monitor.h
 * @brief Component that receives measurement data from the gait monitor.
 *
 * The gait monitor can be used to querry the current gait cycle that can then be added to the meassured data.
 *
 * @author Matthias Becker
 * @date June 12. 2019
 */

#ifndef COMPONENTS_GAIT_MONITOR_H_
#define COMPONENTS_GAIT_MONITOR_H_

#include "driver/spi_master.h"

/** Subject activity level is unknown */
#define BIONICS_ACTIVITY_UNKNOWN                                (((((uint32_t)0))) + ((((uint32_t)0))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is immobile */
#define BIONICS_ACTIVITY_IMMOBILE                               (((((uint32_t)1))) + ((((uint32_t)0))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is mobile */
#define BIONICS_ACTIVITY_MOBILE                                 (((((uint32_t)2))) + ((((uint32_t)0))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is ambulating */
#define BIONICS_ACTIVITY_AMBULATING                             (((((uint32_t)4))) + ((((uint32_t)0))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is sitting */
#define BIONICS_ACTIVITY_SITTING                                (((BIONICS_ACTIVITY_IMMOBILE)) + ((((uint32_t)16))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subjeft is standing */
#define BIONICS_ACTIVITY_STANDING                               (((BIONICS_ACTIVITY_IMMOBILE)) + ((((uint32_t)32))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is changing walking direction */
#define BIONICS_ACTIVITY_U_TURN                                 (((BIONICS_ACTIVITY_MOBILE)) + ((((uint32_t)8))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is sitting and relaxing in a chair */
#define BIONICS_ACTIVITY_CHAIR_RELAX                            (((BIONICS_ACTIVITY_SITTING)) + ((((uint32_t)1))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is exiting from chair */
#define BIONICS_ACTIVITY_CHAIR_EXIT                             (((BIONICS_ACTIVITY_SITTING)) + ((((uint32_t)2))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is walking */
#define BIONICS_ACTIVITY_WALKING                                (((BIONICS_ACTIVITY_AMBULATING)) + ((((uint32_t)1))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is going up stairs */
#define BIONICS_ACTIVITY_UPSTAIRS                               (((BIONICS_ACTIVITY_AMBULATING)) + ((((uint32_t)2))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is going down stairs */
#define BIONICS_ACTIVITY_DOWNSTAIRS                             (((BIONICS_ACTIVITY_AMBULATING)) + ((((uint32_t)4))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)0))<<24))

/** Subject is in stance phase */
#define BIONICS_ACTIVITY_STANCE_PHASE                           (((BIONICS_ACTIVITY_AMBULATING)) + ((((uint32_t)0))<<8) + ((((uint32_t)16))<<16) + ((((uint32_t)0))<<24))

/** Subject is in swing phase */
#define BIONICS_ACTIVITY_SWING_PHASE                            (((BIONICS_ACTIVITY_AMBULATING)) + ((((uint32_t)0))<<8) + ((((uint32_t)32))<<16) + ((((uint32_t)0))<<24))

/** Subject is has double limb support */
#define BIONICS_ACTIVITY_DOUBLE_LIMB_SUPPORT                    (((BIONICS_ACTIVITY_STANCE_PHASE)) + ((((uint32_t)0))<<8) + ((((uint32_t)1))<<16) + ((((uint32_t)0))<<24))

/** Subject has single limb support */
#define BIONICS_ACTIVITY_SINGLE_LIMB_SUPPORT                    (((BIONICS_ACTIVITY_STANCE_PHASE)) + ((((uint32_t)0))<<8) + ((((uint32_t)2))<<16) + ((((uint32_t)0))<<24))

/** Subject is in limb advancement */
#define BIONICS_ACTIVITY_LIMB_ADVANCEMENT                       (((BIONICS_ACTIVITY_SWING_PHASE)) + ((((uint32_t)0))<<8) + ((((uint32_t)1))<<16) + ((((uint32_t)0))<<24))

/** Subject is in heelstrike */
#define BIONICS_ACTIVITY_HEELSTRIKE                             (((BIONICS_ACTIVITY_DOUBLE_LIMB_SUPPORT)) + ((((uint32_t)0))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)1))<<24))

/** Subject is in midstance */
#define BIONICS_ACTIVITY_MIDSTANCE                              (((BIONICS_ACTIVITY_SINGLE_LIMB_SUPPORT)) + ((((uint32_t)0))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)1))<<24))

/** Subject is in toeoff */
#define BIONICS_ACTIVITY_TOEOFF                                 (((BIONICS_ACTIVITY_LIMB_ADVANCEMENT)) + ((((uint32_t)0))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)16))<<24))

/** Subject is in midswing */
#define BIONICS_ACTIVITY_MIDSWING                               (((BIONICS_ACTIVITY_LIMB_ADVANCEMENT)) + ((((uint32_t)0))<<8) + ((((uint32_t)0))<<16) + ((((uint32_t)128))<<24))

/**
 * @brief Initialize the component.
 *
 * @param _spi The handle to the SPI device used.
 * @return ESP_OK if success, ESP_FAIL otherwise.
 */
esp_err_t gait_monitor_init(spi_device_handle_t _spi);



#endif /* COMPONENTS_GAIT_MONITOR_H_ */
