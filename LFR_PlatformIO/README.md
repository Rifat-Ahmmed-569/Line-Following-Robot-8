# LFR / Maze Solver — PlatformIO (Arduino Nano, 8 IR sensors)

Original Arduino sketch by **Rifat Ahmmed**. Ported to PlatformIO for VS Code and
re-mapped from a 6-sensor array to **8 IR sensors on A0–A7**.

## Open in VS Code
1. Install the **PlatformIO IDE** extension.
2. `File → Open Folder…` → this folder (the one with `platformio.ini`).
3. PlatformIO sidebar → `nano_new` → **Build / Upload / Monitor** (new bootloader).
   - Use `nano` only for Nano clones with the old bootloader.
   - Mac + CH340 clone not showing a port → install the CH340 driver.

## Layout
```
platformio.ini
include/config.h        pins, speeds, timings, sensor layout, EEPROM map
include/globals.h       shared state + prototypes
src/main.cpp            setup / loop / button-mode mapping
src/sensors.cpp         reading, calibration, serial debug
src/line_tracking.cpp   8-sensor line tracking + SPEED TABLES (tune here)
src/line_follow.cpp     mode 1
src/maze_scan.cpp       modes 8, 9
src/maze_solving.cpp    modes 5, 10
src/algorithm.cpp       shortest-path reduction
src/recording.cpp       path EEPROM save/load
src/motor.cpp  button.cpp  bluetooth.cpp  servo.cpp
```

## Wiring
| Part | Pin |
|---|---|
| IR sensors | A0 … A7 (A0 = **rightmost** by default) |
| Left motor DIR | D2 (fwd), D4 (back) · PWM D3 |
| Right motor DIR | D5 (fwd), D7 (back) · PWM D6 |
| Button | D12 → GND (internal pull-up) |
| LED | D13 |
| Bluetooth | D0/D1 (unplug HC-05 while uploading) |

If A0 is on the **left** of your array, set `SENSOR_A0_IS_LEFT = true` in `config.h`.

## Button modes (press count)
| Presses | Mode |
|---|---|
| 1 | Line follow |
| 2 | Calibrate (place on line, robot spins ~3.4 s) |
| 3 | Straight-line motor test |
| 4 | Digital sensor reading (serial) |
| 5 | Solve maze from end point |
| 6 | Analog sensor reading (serial) |
| 7 | Bluetooth control |
| 8 | Scan maze → solve back from end |
| 9 | Scan maze, stop at end |
| 10 | Solve maze from start |
| 11 | Print saved path (serial) |

## First run
1. Upload, open Monitor (9600).
2. Press **6×** → push the robot across the line, confirm the left-most number
   in the printout reacts to the left-most sensor. Flip `SENSOR_A0_IS_LEFT` if not.
3. Reset, press **2×** on the line to calibrate.
4. Press **4×** → check that only the sensors over the line read `1`.
5. Press **1×** to run.

## How 6 → 8 sensors was mapped
| 6-sensor original | 8-sensor version |
|---|---|
| `s[2]`, `s[3]` centre | `s[3]`, `s[4]` (`S_CR`, `S_CL`) |
| `s[0]` / `s[5]` outer | `s[0]` / `s[7]` (`S_RIGHT`, `S_LEFT`) |
| `s[1..4]` inner | `s[1..6]` (`inner_sum()`) |
| `sum == 1 or 2` → line | `sum <= LINE_MAX_ACTIVE` (3) |
| `sum == 3..5` → node | `3 < sum < ALL_ON_MIN` |
| `sum == 6` → T / cross / end | `sum >= ALL_ON_MIN` (8) |
| 11 hard-coded patterns × 3 modes | `track_line()` + speed tables |
| EEPROM cal 0–17 | EEPROM cal 0–23, path still at 30 |

`track_line()` finds the contiguous block of active sensors, computes an error of
−7…+7 (half-sensor steps from centre) and looks up motor speeds. Table values were
interpolated from the original 6-sensor speeds — expect to re-tune on track.
Split/noisy patterns are ignored (keep last motor command), like the original.

## Fixes vs the original
- `brake;` → `brake();` in line follow (the brake before a 90° turn never ran).
- `r == 2` was used twice in `loop()` — analog reading moved to **6 presses**.
- `maze_solving(0)` started with `counter = 255` (byte underflow) and read outside
  `path[]` at the first node — now starts at 0.
- Solving after a power cycle now loads the saved path from EEPROM instead of
  running on an empty `path[]`.
- Path writes are bounds-checked; maze scan resets the path each run.
- `motor()` clamps PWM to 0–255.
