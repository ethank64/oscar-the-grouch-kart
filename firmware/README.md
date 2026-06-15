# Firmware

ESP32 firmware: pair the Wii remote over Bluetooth, read tilt + buttons, mix a
differential drive command, and send it over UART to the dual VESC.

> **Status:** written, **not yet compiled or flashed**. Bring it up wheels-off-
> the-ground and tune the constants at the top of [`src/main.cpp`](src/main.cpp).

## Hardware requirement

Must be an **original ESP32 (ESP-WROOM-32)** — the Wiimote needs Bluetooth
Classic, which the ESP32-S2/S3/C3 do **not** have. The board in the BOM is fine.

## Stack

- [`ESP32Wiimote`](https://github.com/bigw00d/Arduino-ESP32Wiimote) — Wiimote over BT Classic
- [`VescUart`](https://github.com/SolidGeek/VescUart) — VESC UART protocol (incl. CAN forwarding)

## Control mapping

| Input | Action |
|-------|--------|
| **B trigger (back)** | Dead-man — held = armed, released = coast/stop |
| Tilt (roll, held sideways) | Steer |
| Button **2** | Gas |
| Button **1** | Brake |

Three independent stops (see [`../docs/electrical.md`](../docs/electrical.md)):
dead-man release, the firmware's `LINK_TIMEOUT_MS`, and the VESC's own UART
timeout (set that in VESC Tool as the hardware backstop).

## Wiring

| ESP32 | VESC (master) |
|-------|---------------|
| GPIO17 (TX2) | UART RX |
| GPIO16 (RX2) | UART TX |
| GND | GND |

The ESP32 is powered from the 48 V→5 V buck (5 V pin), **not** from VESC 5 V.
The two VESCs talk to each other over CAN; only the master connects to the ESP32.

## VESC Tool setup (do this first, per motor)

1. Run **motor detection** for each motor; enable **sensored (hall)** mode.
2. Set conservative **motor + battery current limits** and a **max ERPM** — that
   ERPM is your real top-speed cap (the firmware caps torque, not speed).
3. Set the slave VESC's **CAN id** = `RIGHT_CAN_ID` (default `1` in the code) and
   enable "send status over CAN" on both.
4. Set a **UART/app command timeout** so a lost link brakes/releases on its own.

## Build & flash (PlatformIO)

No toolchain is installed yet. Install one, then:

```bash
pip install platformio        # or: brew install platformio
cd firmware
pio run                       # compile
pio run -t upload             # flash (ESP32 plugged in)
pio device monitor            # serial @ 115200 - watch the debug print
```

If the ESP32 doesn't enumerate as a serial port, install the USB-serial driver
for your board (CP210x or CH340) — a charge-only cable also looks like this.

## Bench bring-up (wheels OFF the ground)

1. Flash, open the serial monitor, press **1+2** on the remote to pair.
2. Watch the `acc[x,y,z]` debug line. Hold the remote level, then roll it fully
   each way. Note the level value and the swing -> set `ACC_CENTER`, `ACC_RANGE`,
   and the steer axis (`acc.yAxis` by default) in `main.cpp`.
3. Hold **B** (dead-man) + **2** (gas): both wheels should spin up smoothly.
   Release B: they coast. If a wheel spins backward, flip its direction in VESC
   Tool. If steering is reversed, flip `STEER_SIGN`.
4. Only raise `MAX_MOTOR_CURRENT_A` / ERPM once the mapping and stops all behave.
