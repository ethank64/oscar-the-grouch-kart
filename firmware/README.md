# Firmware

ESP32 firmware: pair the Wii remote over Bluetooth, read tilt + buttons, mix a
differential drive command, and stream it over UART to the hoverboard FOC
controller.

## Planned stack

- **Board:** ESP32 (Bluetooth Classic for Wiimote HID)
- **Wiimote:** [`ESP32Wiimote`](https://github.com/bigw00d/Arduino-ESP32Wiimote)
- **Motor side:** [`hoverboard-firmware-hack-FOC`](https://github.com/EmanuelFeru/hoverboard-firmware-hack-FOC)
  on the donor controller, UART `(speed, steer)` protocol

## Layout (to come)

- `src/` — control loop, drive mix, failsafes
- See [`../docs/control-scheme.md`](../docs/control-scheme.md) for the mapping
  and tuning knobs.

Nothing here yet — starts in Phase 3.
