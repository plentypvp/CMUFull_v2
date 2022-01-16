#ifndef BLE_CMUS_H__
#define BLE_CMUS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "dsp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_CMUS_DEF(_name)                                                                          \
static ble_cmus_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_LBS_BLE_OBSERVER_PRIO,                                                     \
                     ble_cmus_on_ble_evt, &_name)

#define CMUS_UUID_BASE {0x21, 0xD1, 0xBC, 0xEA, 0x5E, 0x74, 0x23, 0x15, 0xED, 0xEA, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00}
#define CMUS_UUID_SERVICE 0x1523
#define CMUS_UUID_MODE_CHAR 0x1525
#define CMUS_UUID_DSP_CHAR 0x1526

typedef struct ble_cmus_s ble_cmus_t;

typedef void (*ble_cmus_mode_write_handler_t) (uint16_t conn_handle, ble_cmus_t * p_cmus, neopixel_settings_s new_settings);
typedef void (*ble_cmus_dsp_write_handler_t) (uint16_t conn_handle, ble_cmus_t * p_cmus, dsp_settings_s new_settings);

typedef struct
{
    ble_cmus_mode_write_handler_t mode_write_handler;
    ble_cmus_dsp_write_handler_t dsp_write_handler;
} ble_cmus_init_t;

/**@brief CMU Service structure. This structure contains various status information for the service. */
struct ble_cmus_s
{
    uint16_t service_handle; /**< Handle of CMU Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t mode_char_handles; /**< Handles related to the Mode Characteristic. */
    ble_gatts_char_handles_t dsp_char_handles; /**< Handles related to the DSP Characteristic. */
    uint8_t uuid_type; /**< UUID type for the CMU Service. */
    ble_cmus_mode_write_handler_t mode_write_handler;   /**< Event handler to be called when the Mode Characteristic is written. */
    ble_cmus_dsp_write_handler_t dsp_write_handler;   /**< Event handler to be called when the Mode Characteristic is written. */
};


/**@brief Function for initializing the CMU Service.
 *
 * @param[out] p_cmus      CMU Service structure. This structure must be supplied by
 *                        the application. It is initialized by this function and will later
 *                        be used to identify this particular service instance.
 * @param[in] p_cmus_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was initialized successfully. Otherwise, an error code is returned.
 */
uint32_t ble_cmus_init(ble_cmus_t * p_cmus, const ble_cmus_init_t * p_cmus_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the CMU Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  CMU Service structure.
 */
void ble_cmus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

#ifdef __cplusplus
}
#endif

#endif // BLE_CMUS_H__