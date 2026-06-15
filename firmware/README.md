# Firmware

ESP32 firmware: pair the Wii remote over Bluetooth, read tilt + buttons, mix a
differential drive command, and send it over UART to the dual VESC.

## Planned stack

- **Board:** ESP32 (Bluetooth Classic for Wiimote HID)
- **Wiimote:** [`ESP32Wiimote`](https://github.com/bigw00d/Arduino-ESP32Wiimote)
- **Motor side:** dual [VESC](https://github.com/vedderb/bldc) (Flipsky FSESC6.7),
  commanded over UART. Use the VESC UART protocol (e.g. `vesc_uart` /
  `set_current` / `set_rpm`), and set a UART command timeout in VESC Tool so a
  lost link coasts the motors to zero.

## Layout (to come)

- `src/` — control loop, drive mix, failsafes
- See [`../docs/control-scheme.md`](../docs/control-scheme.md) for the mapping
  and tuning knobs.

Nothing here yet — starts in Phase 3.
