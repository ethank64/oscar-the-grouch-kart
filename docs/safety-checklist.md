# Safety checklist

You'll be enclosed, with limited visibility, in a crowd, at a college party.
This is the highest-risk part of the build. None of these are optional.

## Electrical

- [ ] Inline **fuse** on the battery positive lead, sized to the controller.
- [ ] **Main power switch** reachable from inside the can.
- [ ] **E-stop** that kills 36 V independently of the firmware.
- [ ] Proper connectors (XT60 / XT90) — never bare li-ion leads.
- [ ] Battery secured low and protected from puncture/crush.

## Control

- [ ] **Dead-man switch:** motors only enable while a button is held. Release =
      coast/stop. Non-negotiable.
- [ ] **Software speed cap** (~6–8 mph) baked into firmware.
- [ ] **Slew-rate limiting** so a tilt twitch can't become a lurch.
- [ ] Failsafe: lost Bluetooth link → command goes to zero.

## Mechanical / rider

- [ ] **Sit low** — keep the center of gravity down; battery at the very bottom.
      Standing makes it tip on hard turns.
- [ ] Wide enough caster base that pivoting in place doesn't tip it.
- [ ] **Guard the wheels** so feet/hands/costume can't reach the spokes.
- [ ] **Visibility:** eye-level holes or a mesh band — never drive blind.

## Testing discipline

- [ ] Bench-test with wheels off the ground first.
- [ ] Tethered low-speed tests before any rider-at-speed.
- [ ] Helmet on, open area (empty parking lot) for the first full-speed runs.
- [ ] Only take it near people after every box above is checked.
