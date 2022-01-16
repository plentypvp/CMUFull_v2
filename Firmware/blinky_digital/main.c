#include <stdint.h>
#include "app_util_platform.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "bluetooth.h"

#ifdef FPU_INTERRUPT_MODE
/**
 * @brief FPU Interrupt handler. Clearing exception flag at the stack.
 *
 * Function clears exception flag in FPSCR register and at the stack. During interrupt handler
 * execution FPU registers might be copied to the stack (see lazy stacking option) and
 * it is necessary to clear data at the stack which will be recovered in the return from
 * interrupt handling.
 */
void FPU_IRQHandler(void)
{
    // Prepare pointer to stack address with pushed FPSCR register.
    uint32_t * fpscr = (uint32_t * )(FPU->FPCAR + FPU_FPSCR_REG_STACK_OFF);
    // Execute FPU instruction to activate lazy stacking.
    (void)__get_FPSCR();
    // Clear flags in stacked FPSCR register.
    *fpscr = *fpscr & ~(FPU_EXCEPTION_MASK);
}
#endif

int main(void)
{
    // Leds and NeoPixel
    bsp_board_init(BSP_INIT_LEDS);
    neopixel_init();
    neopixel_colorwipe(255, 0, 0);
    nrf_delay_ms(500);

    // ADC and Bluetooth
    saadc_init();
    timers_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();
    advertising_start();
    APP_ERROR_CHECK(sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, 0, 8));

    // Finish NeoPixel, indicating success
    neopixel_set_cmu_color(0, 0, 0);
    #ifdef FPU_INTERRUPT_MODE
        // Enable FPU interrupt
        NVIC_SetPriority(FPU_IRQn, APP_IRQ_PRIORITY_LOWEST);
        NVIC_ClearPendingIRQ(FPU_IRQn);
        NVIC_EnableIRQ(FPU_IRQn);
    #endif

    // Enter main loop.
    while (1)
    {
        __WFE();
    }
}
// PASSES SINEWAVE TEST
// Only things left:
// External ADC, maybe increase resolution of this, but all other options already tried
// Hardware improve: AGC amplifier from 3.3V, 2.0V peak-peak amplitude, without op amp and resistor divider