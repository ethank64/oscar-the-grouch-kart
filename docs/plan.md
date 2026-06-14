# Build plan

## Core idea: a differential-drive robot you sit in

A round trash can is the ideal footprint for **skid steering** (tank-style):
two driven wheels on a center axle, plus a caster wheel front and back for
balance. Both wheels together = drive; different speeds = steer; opposite =
pivot in place. No steering linkage to build, and nothing sticks out past the
round shell.

## Drivetrain: gut a hoverboard

The best shortcut, and the most embedded-friendly path:

- A used hoverboard ($80–150) yields **two 350 W brushless hub motors** (motor
  built into the wheel, no chains/gears), a **36 V battery**, a charger, and a
  controller board — all matched.
- Reflash the controller with the open-source **FOC firmware**
  (`EmanuelFeru/hoverboard-firmware-hack-FOC`). It runs field-oriented control
  on both motors and accepts a simple `(speed, steer)` command pair over
  **UART** — exactly the interface we want. Bonus: smooth low-speed torque and
  regen braking.
- Alternative for more grunt: two 24–36 V scooter motors + chain + a dual BLDC
  controller. More torque headroom, but you build the drivetrain instead of
  repurposing one. For ~160 lb on modest slopes, hoverboard motors are enough
  (see physics below).

## Brain + Wii remote

The Wiimote is just a **Bluetooth HID** device — no Wii needed.

- **ESP32** as the single controller: does Bluetooth Classic *and* the real-time
  motor loop. The `ESP32Wiimote` library reads buttons, accelerometer, and a
  Nunchuk. One chip reads the remote, mixes the drive command, and streams UART
  to the motor board.
- **Raspberry Pi** (Zero 2 W with `cwiid`/`xwiimote`) has the most battle-tested
  Wiimote support — keep as a fallback if pairing fights us.

### Control scheme (Mario Kart Wii)

Hold the remote sideways like the Wii Wheel.

- Accelerometer roll angle → `steer`
- Button `2` = throttle, `1` = brake/reverse (or Nunchuk stick for analog throttle)

```
left_cmd  = throttle + steer
right_cmd = throttle - steer   // clamp both, then slew-rate limit so it can't lurch
```

## Physics sanity check

Assume rider (73 kg) + can + frame + battery ≈ **100 kg**, 6.5" hub wheels
(r ≈ 0.083 m), 2 × 350 W ≈ **700 W rated** (~1.2–1.6 kW peak).

- **Top speed:** ~12 mph unloaded, ~8–10 mph under a rider. Software-cap to
  ~6–8 mph in a crowd.
- **Slope:** on a 10% grade, gravity pulls back ~98 N + ~20 N rolling ≈ 118 N.
  At 3 m/s that's ~350 W — comfortably inside 700 W. Modest slopes (up to
  ~12–15%) at reduced speed are fine.
- **Startup torque on a hill:** ~5 N·m per wheel needed; FOC firmware delivers
  good standstill torque, so it won't stall.

Conclusion: the cheap hoverboard path clears the requirements. Going *faster*
than a run means stepping up to higher-voltage scooter motors.

## Build order

1. Buy hoverboard, tear down, flash FOC firmware, spin both motors on the bench
   over UART.
2. Build the base plate: center axle for the two hub wheels + fore/aft casters +
   a low seat.
3. ESP32 firmware: pair Wiimote → read tilt/buttons → UART drive command. Test
   wheels-up.
4. Integrate into the can; tethered low-speed tests.
5. Decorate (Oscar shell, eye holes), final safety pass, full-speed test in a
   parking lot.
