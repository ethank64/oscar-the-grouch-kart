# Phased tasks

Paste these into GitHub Issues (or a Project board) as you go. Roughly ordered;
later phases depend on earlier ones.

## Phase 1 — Drivetrain bring-up
- [ ] Source and tear down a donor hoverboard
- [ ] Identify the controller board; confirm it's flashable with FOC firmware
- [ ] Flash `EmanuelFeru/hoverboard-firmware-hack-FOC`
- [ ] Spin both hub motors on the bench via UART `(speed, steer)` commands
- [ ] Characterize: top speed, current draw, low-speed torque

## Phase 2 — Mechanical base
- [ ] Design base plate (center axle for hub wheels + fore/aft casters)
- [ ] Confirm caster + plate rated for full loaded weight
- [ ] Mount motors, casters, low seat
- [ ] Mock-fit base inside the trash can

## Phase 3 — Firmware (control)
- [ ] Pair Wii remote to ESP32 (`ESP32Wiimote`); read buttons + accelerometer
- [ ] Implement drive mix (tilt → steer, button → throttle)
- [ ] Add dead-man enable, speed cap, slew limiting, link-loss failsafe
- [ ] Bench test wheels-up end to end (remote → ESP32 → controller → motors)

## Phase 4 — Integration
- [ ] Wire power: battery → fuse → main switch → e-stop → controller; buck → ESP32
- [ ] Mount electronics in the can
- [ ] Tethered low-speed drive test (no rider)
- [ ] First rider test at walking pace, open area

## Phase 5 — Costume + final safety
- [ ] Build Oscar shell; cut eye-level visibility holes
- [ ] Install wheel guards
- [ ] Walk the full safety checklist
- [ ] Full-speed shakedown in an empty parking lot
