#include "gpio.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define MUX_NOE_PIN   GPIO0
#define MUX_SEL_PIN   GPIO1
#define EN0_PIN       GPIO2
#define NFAULT0_PIN   GPIO3
#define EN1_PIN       GPIO4
#define NFAULT1_PIN   GPIO5

void gpio_init(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);

    /* Outputs: MUX_nOE, MUX_SEL, EN_0, EN_1 */
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                    MUX_NOE_PIN | MUX_SEL_PIN | EN0_PIN | EN1_PIN);

    /* Inputs: nFAULT_0, nFAULT_1 (active low, external pull-up to VBUS_DEV) */
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE,
                    NFAULT0_PIN | NFAULT1_PIN);

    /* Default state: mux disabled, both VBUS channels off */
    gpio_set(GPIOA, MUX_NOE_PIN);    /* nOE high = mux disabled */
    gpio_clear(GPIOA, MUX_SEL_PIN);  /* SEL = 0 */
    gpio_clear(GPIOA, EN0_PIN);      /* VBUS0 off */
    gpio_clear(GPIOA, EN1_PIN);      /* VBUS1 off */
}

void gpio_set_mux_sel(bool sel)
{
    if (sel)
        gpio_set(GPIOA, MUX_SEL_PIN);
    else
        gpio_clear(GPIOA, MUX_SEL_PIN);
}

bool gpio_get_mux_sel(void)
{
    return (GPIO_ODR(GPIOA) & MUX_SEL_PIN) != 0;
}

void gpio_set_mux_oe(bool enable)
{
    /* nOE is active low: enable=true -> pin low */
    if (enable)
        gpio_clear(GPIOA, MUX_NOE_PIN);
    else
        gpio_set(GPIOA, MUX_NOE_PIN);
}

bool gpio_get_mux_oe(void)
{
    /* Return true if mux is enabled (nOE pin is low) */
    return (GPIO_ODR(GPIOA) & MUX_NOE_PIN) == 0;
}

void gpio_set_en0(bool enable)
{
    if (enable)
        gpio_set(GPIOA, EN0_PIN);
    else
        gpio_clear(GPIOA, EN0_PIN);
}

bool gpio_get_en0(void)
{
    return (GPIO_ODR(GPIOA) & EN0_PIN) != 0;
}

void gpio_set_en1(bool enable)
{
    if (enable)
        gpio_set(GPIOA, EN1_PIN);
    else
        gpio_clear(GPIOA, EN1_PIN);
}

bool gpio_get_en1(void)
{
    return (GPIO_ODR(GPIOA) & EN1_PIN) != 0;
}

bool gpio_get_fault0(void)
{
    /* nFAULT is active low: return true if fault present */
    return (gpio_get(GPIOA, NFAULT0_PIN) == 0);
}

bool gpio_get_fault1(void)
{
    return (gpio_get(GPIOA, NFAULT1_PIN) == 0);
}
