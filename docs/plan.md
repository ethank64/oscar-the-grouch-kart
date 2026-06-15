# Build plan

## Core idea: a differential-drive robot you sit in

A round trash can is the ideal footprint for **skid steering** (tank-style):
two driven wheels on a center axle, plus a caster wheel front and back for
balance. Both wheels together = drive; different speeds = steer; opposite =
pivot in place. No steering linkage to build, and nothing sticks out past the
round shell.

## Drivetrain: BLDC hub motors + dual VESC

We buy purpose-built parts with a documented control interface, rather than
hacking a hoverboard. This removes the firmware-flash gamble entirely.

- **Two sensored BLDC hub motors** (10", 48 V, ~500 W each). The motor is built
  into the wheel — no chains or gears. Sensored (hall) versions give smooth
  torque from a standstill, which matters with a rider on a slope. 10" wheels
  handle slopes and pavement cracks far better than a hoverboard's 6.5".
- **Dual VESC** (Flipsky FSESC6.7, rated to 13S/60 V). The VESC is the
  open-source gold standard: it runs field-oriented control, does regen braking,
  enforces configurable current limits, streams telemetry, and **accepts a
  `(speed, steer)` command over UART by design** — exactly the interface we
  want from the ESP32. Configure it once in VESC Tool, no reverse engineering.
- **48 V li-ion battery + BMS + charger**, sourced separately (this is the part
  a hoverboard would have given us for free). ~15 Ah is plenty for a party.

> Watch the voltage ratings: a "48 V" pack reads ~54.6 V fully charged. The VESC
> must be a 13S/60 V unit (not 12S/50 V), and the buck converter must accept
> >54 V in. See [`electrical.md`](electrical.md) for the full power path.

## Brain + Wii remote

The Wiimote is just a **Bluetooth HID** device — no Wii needed.

- **ESP32** as the single controller: does Bluetooth Classic *and* the real-time
  motor loop. The `ESP32Wiimote` library reads buttons, accelerometer, and a
  Nunchuk. One chip reads the remote, mixes the drive command, and sends it to
  the VESC over UART. Both run at 3.3 V logic, so no level shifter is needed.
- **Raspberry Pi** (Zero 2 W with `cwiid`/`xwiimote`) is the fallback if Wiimote
  pairing fights us.

### Control scheme (Mario Kart Wii)

Hold the remote sideways like the Wii Wheel.

- Accelerometer roll angle -> `steer`
- Button `2` = throttle, `1` = brake/reverse (or Nunchuk stick for analog throttle)

```
left_cmd  = throttle + steer
right_cmd = throttle - steer   // clamp both, then slew-rate limit so it can't lurch
```

Full mapping and tuning knobs in [`control-scheme.md`](control-scheme.md).

## Physics sanity check

Rider (73 kg) + can + frame + battery ≈ **100 kg**, 10" wheels (r ≈ 0.127 m),
2 × 500 W = **1000 W rated** (~2 kW peak) at 48 V.

- **Top speed:** comfortably into a fast run; software-cap to ~6–8 mph for crowd
  safety.
- **Slope:** on a 10% grade, gravity pulls back ~98 N + ~25 N rolling ≈ 123 N.
  At 3 m/s that's ~370 W — well inside 1000 W, with margin to spare over the
  hoverboard's 700 W. Modest slopes are comfortable, not marginal.
- **Standstill torque on a hill:** sensored FOC delivers strong torque from
  zero, so no stalling at the bottom of a grade.

The added headroom over the hoverboard path is the main payoff of going to 48 V
and 1 kW.

## Build order

1. Bench-bring-up: wire each hub motor to a VESC channel, configure in VESC Tool
   (motor detection, hall sensors, current limits), spin both wheels-up.
2. Build the base plate: coaxial hub-motor axle mounts + fore/aft casters + a
   low seat. Mock-fit inside the can.
3. ESP32 firmware: pair Wiimote -> read tilt/buttons -> UART `(speed, steer)` to
   the VESC. Test wheels-up end to end.
4. Wire the power/safety path (see [`electrical.md`](electrical.md)). Tethered
   low-speed tests, no rider.
5. Decorate (Oscar shell, eye holes), final safety pass, full-speed test in a
   parking lot.
