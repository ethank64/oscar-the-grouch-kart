# Oscar the Grouch Kart

A motorized, rideable trash-can costume — drive it around wirelessly with a Wii
remote (Mario Kart Wii style, tilt to steer). Built on a gutted hoverboard
drivetrain with an ESP32 brain. Target: carry a ~160 lb rider at a brisk pace,
handle modest slopes, and survive a college Halloween party.

## How it works

It's a differential-drive (skid-steer) robot you sit inside:

- Two **hoverboard hub motors** on a center axle, plus fore/aft caster wheels.
- A **Wii remote** pairs to an **ESP32** over Bluetooth HID. Tilt = steer,
  buttons = throttle/brake.
- The ESP32 mixes a `(speed, steer)` command and streams it over **UART** to the
  hoverboard controller, reflashed with open-source **FOC firmware**.
- A **36 V battery** powers the motors; a buck converter feeds the ESP32.

See [`docs/plan.md`](docs/plan.md) for the full architecture and physics, and
[`docs/safety-checklist.md`](docs/safety-checklist.md) before riding it.

## Repo layout

| Path | What's in it |
|------|--------------|
| `docs/` | Build plan, control scheme, safety checklist |
| `firmware/` | ESP32 code (Wiimote → UART drive command) |
| `wiring/` | Wiring diagrams and reference photos |
| `bom.csv` | Bill of materials (parts, cost, status) |
| `tasks.md` | Phased build tasks — paste into GitHub Issues |

## Status

Planning. Nothing built yet — only the trash can is on hand.
