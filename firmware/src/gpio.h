#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

void gpio_init(void);

void gpio_set_mux_sel(bool sel);
bool gpio_get_mux_sel(void);

void gpio_set_mux_oe(bool enable);
bool gpio_get_mux_oe(void);

void gpio_set_en0(bool enable);
bool gpio_get_en0(void);

void gpio_set_en1(bool enable);
bool gpio_get_en1(void);

bool gpio_get_fault0(void);
bool gpio_get_fault1(void);

#endif
