# Phased tasks

Paste these into GitHub Issues (or a Project board) as you go. Roughly ordered;
later phases depend on earlier ones.

## Phase 1 — Drivetrain bring-up
- [ ] Order hub motors (confirm hall-sensor version), dual VESC, 48V battery
- [ ] Wire each hub motor + halls to a VESC channel
- [ ] Run motor detection in VESC Tool; set current limits and sensored mode
- [ ] Spin both wheels-up; verify both directions and regen braking
- [ ] Characterize: top speed, current draw, standstill torque

## Phase 2 — Mechanical base
- [ ] Design base plate (coaxial hub-motor mounts + fore/aft casters)
- [ ] Fabricate/buy dropout brackets so both axles are aligned
- [ ] Confirm caster + plate rated for full loaded weight
- [ ] Mount motors, casters, low seat
- [ ] Mock-fit base inside the trash can

## Phase 3 — Firmware (control)
- [ ] Pair Wii remote to ESP32 (`ESP32Wiimote`); read buttons + accelerometer
- [ ] Implement drive mix (tilt → steer, button → throttle)
- [ ] Add dead-man enable, speed cap, slew limiting, link-loss failsafe
- [ ] Set the VESC UART timeout so lost commands coast to zero
- [ ] Bench test wheels-up end to end (remote → ESP32 → VESC → motors)

## Phase 4 — Integration
- [ ] Wire power per docs/electrical.md: battery → fuse → anti-spark switch → VESC; buck → ESP32
- [ ] Mount electronics in the can
- [ ] Tethered low-speed drive test (no rider)
- [ ] First rider test at walking pace, open area

## Phase 5 — Costume + final safety
- [ ] Build Oscar shell; cut eye-level visibility holes
- [ ] Install wheel guards
- [ ] Walk the full safety checklist
- [ ] Full-speed shakedown in an empty parking lot
