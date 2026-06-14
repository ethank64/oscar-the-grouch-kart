# Control scheme

## Input mapping (Mario Kart Wii style)

Hold the Wii remote sideways like the Wii Wheel.

| Input | Source | Maps to |
|-------|--------|---------|
| Steer | Accelerometer roll angle | `steer` (-1 .. +1) |
| Throttle | Button `2` (or Nunchuk stick Y) | `throttle` (0 .. +1) |
| Brake / reverse | Button `1` | negative `throttle` |
| **Enable (dead-man)** | Held button (e.g. `B` trigger) | motors armed only while held |

## Drive mix (differential)

```
if (!deadman_held) { left_cmd = right_cmd = 0; }   // failsafe
else {
    left_cmd  = throttle + steer;
    right_cmd = throttle - steer;
    clamp(left_cmd, -1, 1);
    clamp(right_cmd, -1, 1);
    slew_limit(left_cmd);   // cap rate of change per loop
    slew_limit(right_cmd);
}
scale to FOC firmware command range, send over UART
```

## Tuning knobs

- `SPEED_CAP` — hard ceiling on commanded speed (start conservative, ~6 mph).
- `SLEW_RATE` — max command change per control loop (smooths lurch).
- `STEER_GAIN` — how aggressively tilt translates to differential.
- `DEADZONE` — ignore small accelerometer noise near level.
- `LINK_TIMEOUT` — if no Wiimote packet in N ms, command → 0.
