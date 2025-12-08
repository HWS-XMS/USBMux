#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <stddef.h>
#include "gpio.h"
#include "usb_cdc.h"
#include "commands.h"

static char cmd_buf[64];
static size_t cmd_len = 0;

static void clock_setup(void)
{
    /* Enable HSI48 for USB */
    rcc_osc_on(RCC_HSI48);
    rcc_wait_for_osc_ready(RCC_HSI48);

    /* Set HSI48 as system clock */
    rcc_set_sysclk_source(RCC_HSI48);
    while ((RCC_CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI48)
        ;

    /* Update the SystemCoreClock variable */
    rcc_ahb_frequency = 48000000;
    rcc_apb1_frequency = 48000000;
}

static void process_incoming_data(void)
{
    char buf[64];
    size_t len = usb_cdc_read(buf, sizeof(buf));

    for (size_t i = 0; i < len; i++) {
        char c = buf[i];

        if (c == '\r' || c == '\n') {
            if (cmd_len > 0) {
                cmd_buf[cmd_len] = '\0';
                commands_process(cmd_buf, cmd_len);
                cmd_len = 0;
            }
        } else if (cmd_len < sizeof(cmd_buf) - 1) {
            cmd_buf[cmd_len++] = c;
        }
    }
}

int main(void)
{
    clock_setup();
    gpio_init();
    usb_cdc_init();

    while (1) {
        usb_cdc_poll();
        process_incoming_data();
    }

    return 0;
}
