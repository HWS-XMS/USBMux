#include "commands.h"
#include "gpio.h"
#include "usb_cdc.h"
#include <stdbool.h>
#include <string.h>

static void send_response(const char *resp)
{
    usb_cdc_write_string(resp);
    usb_cdc_write_string("\r\n");
}

static void send_bool(bool val)
{
    send_response(val ? "1" : "0");
}

static void cmd_status(void)
{
    /* Build response manually to avoid snprintf/newlib bloat */
    char resp[] = "SEL=0 OE=0 EN0=0 EN1=0 F0=0 F1=0";
    resp[4]  = gpio_get_mux_sel() ? '1' : '0';
    resp[9]  = gpio_get_mux_oe() ? '1' : '0';
    resp[15] = gpio_get_en0() ? '1' : '0';
    resp[21] = gpio_get_en1() ? '1' : '0';
    resp[26] = gpio_get_fault0() ? '1' : '0';
    resp[31] = gpio_get_fault1() ? '1' : '0';
    send_response(resp);
}

static void cmd_fault(void)
{
    char resp[] = "0 0";
    resp[0] = gpio_get_fault0() ? '1' : '0';
    resp[2] = gpio_get_fault1() ? '1' : '0';
    send_response(resp);
}

static bool parse_bool_arg(const char *arg, bool *val)
{
    if (arg[0] == '0' && (arg[1] == '\0' || arg[1] == '\r' || arg[1] == '\n')) {
        *val = false;
        return true;
    }
    if (arg[0] == '1' && (arg[1] == '\0' || arg[1] == '\r' || arg[1] == '\n')) {
        *val = true;
        return true;
    }
    return false;
}

void commands_process(const char *cmd, size_t len)
{
    /* Skip empty commands */
    if (len == 0)
        return;

    /* Trim trailing whitespace */
    while (len > 0 && (cmd[len-1] == '\r' || cmd[len-1] == '\n' || cmd[len-1] == ' '))
        len--;
    if (len == 0)
        return;

    bool val;

    /* STATUS? */
    if (len == 7 && strncmp(cmd, "STATUS?", 7) == 0) {
        cmd_status();
        return;
    }

    /* FAULT? */
    if (len == 6 && strncmp(cmd, "FAULT?", 6) == 0) {
        cmd_fault();
        return;
    }

    /* SEL? */
    if (len == 4 && strncmp(cmd, "SEL?", 4) == 0) {
        send_bool(gpio_get_mux_sel());
        return;
    }

    /* SEL 0 / SEL 1 */
    if (len == 5 && strncmp(cmd, "SEL ", 4) == 0) {
        if (parse_bool_arg(&cmd[4], &val)) {
            gpio_set_mux_sel(val);
            return;
        }
    }

    /* OE? */
    if (len == 3 && strncmp(cmd, "OE?", 3) == 0) {
        send_bool(gpio_get_mux_oe());
        return;
    }

    /* OE 0 / OE 1 */
    if (len == 4 && strncmp(cmd, "OE ", 3) == 0) {
        if (parse_bool_arg(&cmd[3], &val)) {
            gpio_set_mux_oe(val);
            return;
        }
    }

    /* EN0? */
    if (len == 4 && strncmp(cmd, "EN0?", 4) == 0) {
        send_bool(gpio_get_en0());
        return;
    }

    /* EN0 0 / EN0 1 */
    if (len == 5 && strncmp(cmd, "EN0 ", 4) == 0) {
        if (parse_bool_arg(&cmd[4], &val)) {
            gpio_set_en0(val);
            return;
        }
    }

    /* EN1? */
    if (len == 4 && strncmp(cmd, "EN1?", 4) == 0) {
        send_bool(gpio_get_en1());
        return;
    }

    /* EN1 0 / EN1 1 */
    if (len == 5 && strncmp(cmd, "EN1 ", 4) == 0) {
        if (parse_bool_arg(&cmd[4], &val)) {
            gpio_set_en1(val);
            return;
        }
    }

    /* Unknown command - no response (silent) */
}
