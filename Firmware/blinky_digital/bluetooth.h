#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "boards.h"
#include "app_timer.h"
#include "ble_cmus.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"

#ifndef BLINKY_DIGITAL_BLUETOOTH_H
#define BLINKY_DIGITAL_BLUETOOTH_H

#define APP_BLE_OBSERVER_PRIO 3
#define APP_BLE_CONN_CFG_TAG 1

#define APP_ADV_INTERVAL 64
#define APP_ADV_DURATION BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED


#define MIN_CONN_INTERVAL MSEC_TO_UNITS(100, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(200, UNIT_1_25_MS)
#define SLAVE_LATENCY 0
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS)

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(20000)
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000)
#define MAX_CONN_PARAMS_UPDATE_COUNT 3

#define DEAD_BEEF 0xDEADBEEF


BLE_CMUS_DEF(m_cmus);
NRF_BLE_GATT_DEF(m_gatt);
NRF_BLE_QWR_DEF(m_qwr);

void ble_stack_init(void);
void conn_params_init(void);
void services_init(void);
void gatt_init(void);
void gap_params_init(void);
void timers_init(void);
void advertising_init(void);
void advertising_start(void);
#endif //BLINKY_DIGITAL_BLUETOOTH_H