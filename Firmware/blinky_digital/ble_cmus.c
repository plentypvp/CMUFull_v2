#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_LBS)
#include "ble_cmus.h"
#include "ble_srv_common.h"

/**@brief Function for handling the Write event.
 *
 * @param[in] p_cmus CMU Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
static void on_write(ble_cmus_t *p_cmus, ble_evt_t const *p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if ((p_evt_write->handle == p_cmus->mode_char_handles.value_handle)
        && (p_evt_write->len == sizeof(neopixel_settings_s))
        && (p_cmus->mode_write_handler != NULL))
    {
        neopixel_settings_s new_settings = {};
        memcpy(&new_settings, p_evt_write->data, sizeof(neopixel_settings_s));
        p_cmus->mode_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_cmus, new_settings);

        // Change mode
        neopixel_set_settings(new_settings);
    }

    if ((p_evt_write->handle == p_cmus->dsp_char_handles.value_handle)
        && (p_evt_write->len == sizeof(dsp_settings_s))
        && (p_cmus->dsp_write_handler != NULL))
    {
        dsp_settings_s new_settings = {};
        memcpy(&new_settings, p_evt_write->data, sizeof(dsp_settings_s));
        p_cmus->dsp_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_cmus, new_settings);

        // Change settings
        dsp_set_settings(new_settings);
    }
}


void ble_cmus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_cmus_t* p_cmus = (ble_cmus_t*)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_cmus, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

uint32_t ble_cmus_init(ble_cmus_t* p_cmus, const ble_cmus_init_t* p_cmus_init)
{
    uint32_t err_code;
    ble_uuid_t ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_cmus->mode_write_handler = p_cmus_init->mode_write_handler;
    p_cmus->dsp_write_handler = p_cmus_init->dsp_write_handler;

    // Add service.
    ble_uuid128_t base_uuid = {CMUS_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_cmus->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_cmus->uuid_type;
    ble_uuid.uuid = CMUS_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cmus->service_handle);
    VERIFY_SUCCESS(err_code);

    // Copy initial settings structure to buffer
    neopixel_settings_s initial_neopixel_settings = neopixel_get_settings();
    uint8_t initial_value_mode[sizeof(neopixel_settings_s)] = {};
    memcpy(initial_value_mode, &initial_neopixel_settings, sizeof(neopixel_settings_s));

    // Add Mode characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = CMUS_UUID_MODE_CHAR;
    add_char_params.uuid_type = p_cmus->uuid_type;
    add_char_params.init_len = sizeof(neopixel_settings_s);
    add_char_params.max_len = sizeof(neopixel_settings_s);
    add_char_params.char_props.read = 1;
    add_char_params.char_props.write = 1;
    add_char_params.p_init_value = initial_value_mode;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    err_code = characteristic_add(p_cmus->service_handle, &add_char_params, &p_cmus->mode_char_handles);
    VERIFY_SUCCESS(err_code);

    // Copy initial settings structure to buffer
    dsp_settings_s initial_dsp_settings = dsp_get_settings();
    uint8_t initial_value_dsp[sizeof(dsp_settings_s)] = {};
    memcpy(initial_value_dsp, &initial_dsp_settings, sizeof(dsp_settings_s));

    // Add DSP characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid = CMUS_UUID_DSP_CHAR;
    add_char_params.uuid_type = p_cmus->uuid_type;
    add_char_params.init_len = sizeof(dsp_settings_s);
    add_char_params.max_len = sizeof(dsp_settings_s);
    add_char_params.char_props.read = 1;
    add_char_params.char_props.write = 1;
    add_char_params.p_init_value = initial_value_dsp;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    return characteristic_add(p_cmus->service_handle, &add_char_params, &p_cmus->dsp_char_handles);
}

#endif // NRF_MODULE_ENABLED(BLE_LBS)