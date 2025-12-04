PROJECT = usbmux
BUILD_DIR = build

OPENCM3_DIR = libopencm3
DEVICE = stm32f042f4p6
OOCD_INTERFACE = stlink
OOCD_TARGET = stm32f0x

CFILES = src/main.c src/gpio.c src/usb_cdc.c src/commands.c
INCLUDES = -Isrc

CFLAGS += -Os -ggdb3
CFLAGS += -Wall -Wextra -Wshadow -Wno-unused-parameter
CFLAGS += -fno-common -ffunction-sections -fdata-sections

LDFLAGS += -static -nostartfiles
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

.PHONY: all clean flash program

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(PROJECT).elf: $(CFILES) $(LDSCRIPT) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) $(INCLUDES) $(LDFLAGS) -T$(LDSCRIPT) $(CFILES) $(LDLIBS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

flash: $(BUILD_DIR)/$(PROJECT).elf
	openocd -f interface/$(OOCD_INTERFACE).cfg -f target/$(OOCD_TARGET).cfg \
		-c "program $< verify reset exit"

program: $(BUILD_DIR)/$(PROJECT).bin
	dfu-util -a 0 -s 0x08000000:leave -D $<

clean:
	rm -rf $(BUILD_DIR)

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk
