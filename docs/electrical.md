# Electrical & safety wiring

The power path, and how the kill mechanisms actually work. The goal: every way
of stopping the kart is independent and fails safe.

## Main power path

```
48V battery (+ BMS)
  └─[ XT90 ]── ANL/MIDI fuse (60A) ── anti-spark main switch ──┬── Dual VESC ──┬── hub motor L
                                                              │               └── hub motor R
                                                              └── buck 48V→5V ── ESP32
```

- **Fuse:** ANL/MIDI on the battery positive, ~60A. *Not* an ATC blade fuse —
  dual-motor peaks exceed the ~40A blade ceiling.
- **Main switch:** an anti-spark switch (AS150-style), which is rated to make and
  break ~54 V DC under load. An automotive toggle rated "50A 12V DC" is **not** —
  at 48 V the arc won't self-extinguish and the contacts can weld. Use the toggle
  only as a no-load disconnect, if at all.
- **Buck converter:** must accept the pack's full charged voltage (~54.6 V), so
  an 8–60 V module. It powers the ESP32's 5 V rail.

## The three independent stops

1. **Dead-man button (normal use):** the ESP32 only sends nonzero commands while
   a button is held. Release → it commands zero. This is the everyday stop.
2. **UART timeout (link loss):** VESC Tool is configured with a command timeout.
   If the ESP32 stops sending (crash, dead battery, BT drop), the VESC coasts the
   motors to zero on its own — no ESP32 needed.
3. **E-stop (hardware):** the mushroom button's NC contact cuts power to the buck
   (and thus the ESP32). The ESP32 dies → UART goes silent → the VESC's timeout
   (stop #2) brings the motors down. The e-stop does **not** try to break full
   motor current directly — a 10 A button can't — it just kills the brain.

Stops 1 and 2 are software/firmware; stop 3 is hardware and works even if the
firmware is wedged. The anti-spark main switch is the full disconnect for
service/charging.

## Signal wiring

- ESP32 UART TX/RX ↔ VESC UART RX/TX (both 3.3 V logic — no level shifter).
- Common ground between ESP32 and VESC.
- Hall sensor cables from each hub motor into its VESC channel (sensored mode).

## To verify on the bench

- [ ] Fuse value vs. measured peak current during a hard start
- [ ] Anti-spark switch actually interrupts under load without arcing
- [ ] UART timeout triggers a coast-to-zero when ESP32 is unplugged
- [ ] E-stop kills the ESP32 and motors stop within the timeout window
