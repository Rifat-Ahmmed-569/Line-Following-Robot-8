# 🤖 LFR / Maze Solver Robot — Arduino Nano + 8 IR Sensors

A line-following and maze-solving robot. It follows a black line, takes 90° turns, handles crosses and T-junctions, U-turns at dead ends, **learns a maze**, and then runs the **shortest path**.




> **Goal of this README:** if you have never built a robot before, you should still be able to copy this one: buy the parts, wire them, upload the code, and run it.

---

## 📋 Table of Contents
1. [What You Need (Parts List)](#1-what-you-need-parts-list)
2. [Tools You Need](#2-tools-you-need)
3. [How It Works (30-second version)](#3-how-it-works-30-second-version)
4. [Wiring Diagram](#4-wiring-diagram)
5. [Build Steps](#5-build-steps)
6. [Software Setup](#6-software-setup)
7. [First Run (Do This in Order)](#7-first-run-do-this-in-order)
8. [Button Modes](#8-button-modes)
9. [Making the Track](#9-making-the-track)
10. [Tuning](#10-tuning)
11. [Troubleshooting](#11-troubleshooting)
12. [Project Files](#12-project-files)
13. [Known Limitations](#13-known-limitations)
14. [Credits](#14-credits)

---

## 1. What You Need (Parts List)

| # | Part | Qty | Notes |
|---|---|---|---|
| 1 | **Arduino Nano** (ATmega328P) | 1 | Genuine or CH340 clone both work |
| 2 | **TB6612FNG** dual motor driver | 1 | Small, efficient, up to ~1.2 A per motor |
| 3 | **8-channel analog IR sensor array** | 1 | Must have **analog** outputs (e.g. QTR-8A style or 8× TCRT5000 board) |
| 4 | **N20 micro gear motors** + brackets | 2 | 6 V, roughly 300–1000 RPM |
| 5 | Wheels for N20 (≈ 30–45 mm) | 2 | |
| 6 | Caster ball / ball transfer wheel | 1 | Front or rear support |
| 7 | **2S Li-ion / LiPo battery (7.4 V)** | 1 | Example: 2× 18650 in a holder, or a small 2S LiPo |
| 8 | Power switch | 1 | Between battery + and the robot |
| 9 | Push button (tactile) | 1 | Mode selection |
| 10 | LED + 220 Ω resistor | 1 | Status light (optional; the Nano's onboard LED on D13 also works) |
| 11 | Chassis | 1 | Acrylic / PCB / 3D printed, anything flat and light |
| 12 | Jumper wires, header pins, perfboard | — | |
| 13 | HC-05 Bluetooth module | 1 | **Optional**, only for Bluetooth control mode |
| 14 | Mini-USB cable | 1 | For uploading code to the Nano |
| 15 | TCRT5000 | IR sensor |
| 16 | 220ohm and 4.7k ohm resistor | For ir sensors |
| 17 |7805V | Voltage Droping | Converting 12v-7v to constant 5/4.99v |
| 18 | XL6009 | Step-up Boost DC-DC Converter Module |

---
<img width="1600" height="902" alt="bee17d1c-a034-49f1-a7bf-3950b31885d5" src="https://github.com/user-attachments/assets/1877a8d5-13c6-4dcd-a0c0-1d82c701ac6e" />
---

> ⚠️ The TB6612FNG motor supply (VM) is limited to **13.5 V max**. A 2S (7.4 V, 8.4 V when full) battery is the safe choice. 3S also works but is close to the limit.

---

## 2. Tools You Need
- Soldering iron + solder
- Wire cutter / stripper
- Multimeter (very helpful for checking power)
- Double-sided tape / screws / hot glue
- A computer with **VS Code + PlatformIO** (setup in [Section 6](#6-software-setup))

---

## 3. How It Works (30-second version)

```
 8 IR sensors ──► Arduino Nano ──► TB6612FNG ──► 2 motors
   (see line)      (decides)       (power)       (move)
```

- Each IR sensor reads a number (0–1023). Black line = high number, white floor = low number.
- **Calibration** records the min/max of every sensor and saves a threshold to EEPROM, so it stays saved after power off.
- The code works out **where the line is** (left, centre, or right) and sets the motor speeds to steer back to the centre.
- If **many sensors** see black, the robot is at a **junction** (turn, cross, T, or end box).
- In maze mode, every decision at a junction is saved (Left / Straight / Right / U-turn). The robot then removes the dead ends to get the **shortest path** and saves it to EEPROM.

---

## 4. Wiring Diagram

### 4.1 Full system

```
                         ┌──────────────────────────────┐
   2S Battery (7.4V)     │                              │
   (+)──[SWITCH]──┬──────┤ VIN          ARDUINO NANO    │
                  │      │                              │
                  │      │ 5V ───────────┬──────────────┼──► Sensor array VCC
                  │      │               └──────────────┼──► TB6612 VCC + STBY
                  │      │                              │
                  │      │ A0..A7 ◄─────────────────────┼─── Sensor outputs (8 wires)
                  │      │                              │
                  │      │ D2  ─────────────────────────┼──► TB6612 AIN1  (Left fwd)
                  │      │ D4  ─────────────────────────┼──► TB6612 AIN2  (Left back)
                  │      │ D3  (PWM) ───────────────────┼──► TB6612 PWMA  (Left speed)
                  │      │                              │
                  │      │ D7  ─────────────────────────┼──► TB6612 BIN1  (Right fwd)
                  │      │ D5  ─────────────────────────┼──► TB6612 BIN2  (Right back)
                  │      │ D6  (PWM) ───────────────────┼──► TB6612 PWMB  (Right speed)
                  │      │                              │
                  │      │ D12 ──────[BUTTON]───────────┼──► GND
                  │      │ D13 ──[220Ω]──[LED+  LED-]───┼──► GND
                  │      │                              │
                  │      │ D0 (RX) ◄────────────────────┼─── HC-05 TX   (optional)
                  │      │ D1 (TX) ──[voltage divider]──┼──► HC-05 RX   (optional)
                  │      │                              │
                  │      │ GND ─────────────────────────┼──► COMMON GND (everything)
                  │      └──────────────────────────────┘
                  │
                  └──────────────────────────────────────► TB6612 VM (motor power)

   Battery (−) ───────────────────────────────────────────► COMMON GND
```

## Circuit Schematics

[View Full Circuit Schematic (PDF)](Schematic_LFR-schemetics_2026-11-19.pdf)
### 4.2 TB6612FNG motor driver

<img width="628" height="367" alt="4f061709ab0175bec73829e493754e4b83639a31" src="https://github.com/user-attachments/assets/81093475-9438-45a0-a3f9-ada94a6176ce" />


| TB6612FNG pin | Connect to | Purpose |
|---|---|---|
| VM | Battery + (after switch) | Motor power |
| VCC | Nano 5V | Logic power |
| GND (all) | Common GND | |
| STBY | Nano 5V | Enables the driver |
| AIN1 | D2 | Left motor forward |
| AIN2 | D4 | Left motor backward |
| PWMA | D3 | Left motor speed |
| BIN1 | D7 | Right motor forward |
| BIN2 | D5 | Right motor backward |
| PWMB | D6 | Right motor speed |
| AO1 / AO2 | Left motor | |
| BO1 / BO2 | Right motor | |

### 4.3 Sensor array (front view, looking from the back of the robot)

This is the circuit diagram for each sensor:
***
<img width="1342" height="510" alt="image" src="https://github.com/user-attachments/assets/31c9df06-c7a2-4f1f-a929-85e6d20176a7" />

***

```
        ROBOT FRONT  ↑  (direction of travel)


   LEFT                                     RIGHT
   ┌────┬────┬────┬────┬────┬────┬────┬────┐
   │ A7 │ A6 │ A5 │ A4 │ A3 │ A2 │ A1 │ A0 │
   └────┴────┴────┴────┴────┴────┴────┴────┘
    s[7] s[6] s[5] s[4] s[3] s[2] s[1] s[0]
   outer          centre centre          outer

```

<img width="1600" height="902" alt="ec20f666-928f-4a72-a2e4-c38a6448e84b" src="https://github.com/user-attachments/assets/5fa6ca57-3969-4e2e-9330-1c827b3ca0cf" />


- **Default: A0 = RIGHTMOST sensor, A7 = LEFTMOST sensor.**
- If your array is wired the other way round (A0 on the left), you do **not** need to rewire. Just set this in `include/config.h`:
  ```cpp
  constexpr bool SENSOR_A0_IS_LEFT = true;
  ```
- Sensor array VCC → 5V, GND → GND. If your array has an IR LED enable pin (LEDON / CTRL), connect it to 5V.

### 4.4 Full pin summary

| Nano pin | Goes to |
|---|---|
| A0–A7 | IR sensor outputs (A0 = right, A7 = left) |
| D2 | TB6612 AIN1 (left forward) |
| D4 | TB6612 AIN2 (left backward) |
| D3 | TB6612 PWMA (left speed) |
| D7 | TB6612 BIN1 (right forward) |
| D5 | TB6612 BIN2 (right backward) |
| D6 | TB6612 PWMB (right speed) |
| D12 | Button → GND (internal pull-up, no resistor needed) |
| D13 | Status LED (+ 220 Ω) |
| D0 / D1 | HC-05 Bluetooth (optional) |
| VIN | Battery + (7–12 V) |
| 5V | Sensors VCC, TB6612 VCC, STBY |
| GND | Common ground |

> ✅ **Rule #1: ALL grounds must be connected together**: battery −, Nano GND, TB6612 GND, sensor GND. Most "nothing works" problems come from a missing ground.

---

## 5. Build Steps

1. **Mount the motors** at the back of the chassis, one on each side, wheels facing outward.
2. **Mount the caster ball** at the front (or the back) so the robot sits level.
3. **Mount the sensor array** at the very front, facing down.
   - Height above the floor: **about 3–8 mm** (check your sensor's datasheet).
   - Keep it parallel to the floor and centred left-to-right.
4. **Mount the Nano and TB6612FNG** on perfboard in the middle of the chassis.
5. **Mount the battery** low and near the centre for good balance.
6. **Wire everything** exactly as in [Section 4](#4-wiring-diagram).
7. **Before turning on:** use a multimeter to check there is no short between battery + and GND.
8. Turn the switch on. The Nano power LED should light up.

> 💡 Upload the code over USB **with the battery switch OFF** the first time, so the robot doesn't drive off the table.

---

## 6. Software Setup

### 6.1 Install
1. Install **[VS Code](https://code.visualstudio.com/)**.
2. In VS Code, open the **Extensions** tab and install **PlatformIO IDE**.
3. Download or clone this repository.
4. VS Code → `File → Open Folder…` → choose the folder that has `platformio.ini` in it.
5. Wait for PlatformIO to finish downloading the toolchain (first time only).

### 6.2 Choose the right board environment

| Your Nano | Use environment |
|---|---|
| Genuine / newer Nano (new bootloader, 115200 baud) | `nano_new` (default) |
| Old CH340 clone (old bootloader, 57600 baud) | `nano` |

If upload fails with `nano_new`, try `nano`.

### 6.3 Set your COM port
In `platformio.ini`, the `nano_new` environment has:
```ini
upload_port = COM4
```
Change `COM4` to your own port:
- **Windows:** check Device Manager → Ports (e.g. `COM3`, `COM5`)
- **Mac:** something like `/dev/cu.usbserial-1410` or `/dev/cu.wchusbserial1410`
- **Linux:** something like `/dev/ttyUSB0`

Or delete that line and PlatformIO will try to find the port itself.

> 🍎 **Mac + CH340 clone not showing up?** Install the CH340 driver.

### 6.4 Build & Upload
PlatformIO sidebar → `nano_new` →
- **Build** = compile
- **Upload** = send to Nano
- **Monitor** = open the serial monitor (9600 baud)

Or in the terminal:
```bash
pio run                  # build
pio run -t upload        # upload
pio device monitor       # serial monitor
```

> ⚠️ **Unplug the HC-05 Bluetooth module while uploading.** It uses the same D0/D1 pins as USB.

---

## 7. First Run (Do This in Order)

The robot has **one button**. You choose a mode by **pressing the button N times quickly**, then waiting 1 second. The LED blinks with each press.

### Step 1: Check the sensor order (6 presses)
1. Plug in USB, open **Monitor** (9600).
2. Press the button **6 times**.
3. You'll see 8 numbers printed **LEFT → RIGHT**.
4. Move a black strip under the **leftmost** sensor. The **leftmost** number should change.
   - ❌ If the rightmost number changes instead, set `SENSOR_A0_IS_LEFT = true` in `config.h` and upload again.
5. Press the Nano's **reset** button to exit.

### Step 2: Calibrate (2 presses)
1. Put the robot **on the track, centred over the black line**.
2. Press the button **2 times**.
3. The robot **spins in place for ~3.4 s** so every sensor sees both black and white.
4. Calibration is saved to EEPROM, so you only redo this if the lighting or floor changes.

### Step 3: Check the digital readings (4 presses)
1. Press **reset**, then press the button **4 times**.
2. Monitor shows `0` / `1` for each sensor (LEFT → RIGHT).
3. Only the sensors **over the black line** should show `1`.
   - ❌ If it's backwards (white = 1), see [Troubleshooting](#11-troubleshooting).

### Step 4: Check the motors (3 presses)
1. Press **reset**, lift the robot (wheels in the air), press **3 times**.
2. **Both wheels should spin FORWARD.**
   - ❌ If a wheel spins backward, **swap that motor's two wires** (AO1↔AO2 or BO1↔BO2).
   - ❌ If a wheel doesn't spin, check STBY is on 5V and VM has battery power.

### Step 5: Run! (1 press)
1. Press **reset**, place the robot on the line, press **1 time**.
2. It follows the line. 🎉

---

## 8. Button Modes

Press the button **N times**, then wait 1 second.

| Presses | Mode | What it does |
|---|---|---|
| **1** | Line follow | Fast line following with 90° turns, crosses, U-turns |
| **2** | Calibrate | Spins over the line, saves thresholds to EEPROM |
| **3** | Motor test | Both motors forward (check direction) |
| **4** | Digital reading | Prints 0/1 per sensor to serial |
| **5** | Solve from end | Runs the saved shortest path backwards (end → start) |
| **6** | Analog reading | Prints raw sensor values to serial |
| **7** | Bluetooth | RC car control from a phone app |
| **8** | Scan + solve back | Explores the maze, then solves back from the end |
| **9** | Scan only | Explores the maze, saves shortest path, stops at end |
| **10** | Solve from start | Runs the saved shortest path from the start |
| **11** | Print path | Prints the saved maze path to serial |

> To leave any mode, press the Nano's **reset** button.

### Maze workflow
1. Put the robot at the maze **start**, press **9×** → it explores using the right-hand rule and stops at the end box.
2. Carry it back to the start, press **10×** → it drives the **shortest path**.
3. The path stays saved after power off. Press **11×** to see it.

Path codes printed in mode 11:

| Code | Meaning |
|---|---|
| 1 | Left |
| 2 | Straight |
| 3 | Right |
| 4 | U-turn |
| 5 | End |
| 6 | Grab (gripper) |
| 7 | Release (gripper) |

> Want the left-hand rule instead? In `src/main.cpp` change `char side = 'r';` to `char side = 'l';`

### Bluetooth mode (7 presses)
Use any "Arduino Bluetooth RC Car" phone app. Pair with HC-05 (default PIN `1234` or `0000`).

| Key | Action |
|---|---|
| F / B | Forward / Backward |
| L / R | Spin left / right |
| G / I | Forward-left / Forward-right |
| H / J | Back-right / Back-left |
| S | Stop |
| 0–9 | Speed |
| U / u | LED on / off |

---

## 9. Making the Track

- **Floor:** white (chart paper, flex banner, or white board).
- **Line:** black electrical tape, about **18–25 mm wide**.
- A normal straight line should cover **2–3 sensors** at once.
- **End box:** a solid black square wider than the sensor array (the robot stops if all 8 sensors stay black for 0.6 s).
- Make 90° corners clean; avoid wrinkles and shiny tape reflections.

---

## 10. Tuning

Everything you might tune lives in **two files**:

### `include/config.h`
| Setting | Default | What it does |
|---|---|---|
| `spl` / `spr` | 10 | Left / right speed multiplier. If the robot drifts on a straight line, lower the faster side by 1 |
| `node_delay` | 29 ms | How far it moves past a junction before checking |
| `stop_timer` | 600 ms | How long all-black must last to count as the end box |
| `u_turn_delay` | 40 ms | How long the line must be lost before a U-turn |
| `turn_brake` | 17 ms | Counter-spin after a turn to stop overshoot |
| `brake_time` | 20 ms | Reverse pulse when braking |
| `LINE_MAX_ACTIVE` | 3 | Max sensors on a plain line. Raise it for a wider line |
| `ALL_ON_MIN` | 8 | Sensors needed for T / cross / end. Use **7** if one sensor is flaky |

### `src/line_tracking.cpp`: speed tables
- `FAST` = line follow mode, `MAZE` = maze modes.
- Values are multiplied by `spl` / `spr` (e.g. `10 × 10 = 100` PWM out of 255).
- `step 0` = line in the centre, `step 7` = line at the far edge.
- **Wobbles a lot?** Make the `Inner` values change more gently (e.g. `10, 9, 9, 8, 6...`).
- **Misses sharp curves?** Make the outer steps more negative (e.g. `-4 → -6`).
- **Want it faster?** Raise `straight` and the `Outer` values a little at a time.

> 💡 Change **one thing at a time**, upload, and test.

---

## 11. Troubleshooting

| Problem | Fix |
|---|---|
| Nothing happens at all | Check battery, switch, and that **all GNDs are connected** |
| Motors don't move | TB6612 **STBY must be on 5V**; check VM has battery voltage |
| One wheel spins backward | Swap that motor's two wires |
| Robot turns the wrong way at junctions | Sensor order is flipped → toggle `SENSOR_A0_IS_LEFT` |
| White floor reads `1`, black line reads `0` | Your sensors give lower values on black. In `src/sensors.cpp`, `reading()`, change `>` to `<` in the `analogRead(...) > mid[i]` line, or use a white line on black |
| Serial says "Not calibrated" | Press button **2×** on the line |
| All sensors always 0 or always 1 | Sensor too high/low from the floor, or array not powered. Check with mode **6** |
| Upload fails | Unplug HC-05; try the `nano` environment; check `upload_port` |
| Robot overshoots turns | Increase `turn_brake` or lower turn speeds |
| Robot stops at a crossing like it's the end | Increase `stop_timer` |
| Robot drifts on straights | Adjust `spl` / `spr` |
| "No saved path" in modes 5/10 | Run a maze scan (**9×**) first |

---

## 12. Project Files

```
platformio.ini          board + upload settings
upload_reset.py         auto-reset helper for uploading
include/config.h        pins, speeds, timings, sensor layout, EEPROM map  ← tune here
include/globals.h       shared variables + function list
src/main.cpp            setup(), loop(), button → mode mapping
src/sensors.cpp         sensor reading, calibration, serial debug
src/line_tracking.cpp   line-position logic + SPEED TABLES               ← tune here
src/line_follow.cpp     mode 1
src/maze_scan.cpp       modes 8, 9
src/maze_solving.cpp    modes 5, 10
src/algorithm.cpp       shortest-path reduction
src/recording.cpp       save / load path in EEPROM
src/motor.cpp           motor control + braking
src/button.cpp          button press counter
src/bluetooth.cpp       mode 7
src/servo.cpp           gripper hooks (empty, add your servo code)
```

### EEPROM map
| Address | Content |
|---|---|
| 0–7 | Sensor thresholds (mid) |
| 8–15 | Sensor max |
| 16–23 | Sensor min |
| 30–79 | Saved maze path (ends with `5`) |

---

## 13. Known Limitations
- **Mode 8** does not do the 180° turn at the end yet (`rotate_backwards()` in `motor.cpp` is empty). Turn the robot around by hand, or use mode 9 + mode 5.
- **Gripper** (path codes 6/7) is only a placeholder; add your own servo code in `src/servo.cpp`.
- Speed tables were adapted from a 6-sensor version, so expect to fine-tune them on your track.

---

## Project Demonstration


## Video Demonstration

[![Line Following Robot Demonstration](https://img.youtube.com/vi/oyRrVLPR2Yk/maxresdefault.jpg)](https://youtube.com/shorts/oyRrVLPR2Yk)

*Click the thumbnail to watch the robot in action on YouTube.*


<p align="center">
  <b>▶ Watch the Line Following Robot in Action on YouTube</b>
</p>

---
If you build one, star ⭐ the repo and share a video!
