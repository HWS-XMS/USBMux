# USBMux

USB multiplexer for switching a USB device between two hosts, with VBUS power control.

## Hardware

- **MCU**: STM32F042F4P6 (USB CDC interface)
- **USB Mux**: FSUSB42MUX
- **VBUS Switch**: 2x TPS2051CDBV (with overcurrent/thermal protection)

KiCad design files in `hardware/`.

## Firmware

### Build

```bash
cd firmware
git submodule update --init --recursive
# Apply libopencm3 fix for STM32F042 USB bug
cd libopencm3 && patch -p1 < ../libopencm3-stm32f042-usb-fix.patch && cd ..
make -C libopencm3 TARGETS="stm32/f0" -j$(nproc)
make
```

**Note**: The patch fixes a critical bug in libopencm3's `st_usbfs_copy_from_pm()` function that causes data corruption when receiving odd-length USB packets on STM32F042. This bug has been reported to the libopencm3 project.

### Flash

```bash
make flash    # via ST-Link
make program  # via DFU mode
```

### Commands (USB CDC)

| Command | Description |
|---------|-------------|
| `SEL 0` / `SEL 1` | Select host 0 or 1 |
| `SEL?` | Query selection |
| `OE 0` / `OE 1` | Disable/enable mux |
| `OE?` | Query mux enable |
| `EN0 0` / `EN0 1` | Disable/enable VBUS channel 0 |
| `EN0?` | Query VBUS 0 |
| `EN1 0` / `EN1 1` | Disable/enable VBUS channel 1 |
| `EN1?` | Query VBUS 1 |
| `FAULT?` | Query fault flags |
| `STATUS?` | Query all states |

Responses end with `\r\n`.

## Software

Python wrapper in `software/usbmux.py`.

```python
from usbmux import USBMux

with USBMux() as mux:      # auto-detects device
    mux.sel = 0            # select host 0
    mux.oe = True          # enable mux
    mux.en0 = True         # enable VBUS 0 (raises USBMuxFaultError on fault)
    print(mux.status)
```

Requires `pyserial`:
```bash
pip install pyserial
```
