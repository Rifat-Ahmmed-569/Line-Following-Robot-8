#pragma once
// ============================================================================
//  CONFIG — pins, speeds, timings, sensor layout.
//  Line-tracking speed tables live at the top of src/line_tracking.cpp.
// ============================================================================
#include <Arduino.h>

// ---------------- Motor driver (DIR + PWM per motor) ----------------
constexpr uint8_t left_motor_forward   = 2;
constexpr uint8_t left_motor_backward  = 4;
constexpr uint8_t right_motor_forward  = 7;
constexpr uint8_t right_motor_backward = 5;
constexpr uint8_t left_motor_speed     = 3;   // PWM
constexpr uint8_t right_motor_speed    = 6;   // PWM

// ---------------- UI ----------------
constexpr uint8_t light = 13;   // status LED
constexpr uint8_t sw    = 12;   // mode button (INPUT_PULLUP, active LOW)

// ---------------- Speed coefficients ----------------// ---------------- Speed coefficients ----------------
constexpr int spl = 10;   // left motor speed coefficient  (old alt: 21)
constexpr int spr = 10;   // right motor speed coefficient (old alt: 21)
constexpr int CAL_SPEED = 120;   // calibration spin PWM (was 50)

// ---------------- Timings (ms) ----------------
constexpr unsigned long node_delay   = 29;
constexpr unsigned long stop_timer   = 600;
constexpr unsigned long u_turn_delay = 40;
constexpr unsigned long turn_brake   = 17;
constexpr unsigned long brake_time   = 20;   // old alt: 130
constexpr unsigned long Ubrake_time  = 1;

// ---------------- Sensor array: 8x IR on A0..A7 ----------------
// Index convention (kept from the original code):
//   s[0] = RIGHTMOST sensor ... s[7] = LEFTMOST sensor
// By default A0 = rightmost, A7 = leftmost.
// If your board has A0 on the LEFT, set this to true. Verify with mode 6
// (analog reading): the printout is ordered LEFT -> RIGHT.
constexpr bool SENSOR_A0_IS_LEFT = false;

constexpr uint8_t SENSOR_COUNT = 8;
constexpr uint8_t S_RIGHT = 0;   // rightmost (outer)
constexpr uint8_t S_CR    = 3;   // centre-right
constexpr uint8_t S_CL    = 4;   // centre-left
constexpr uint8_t S_LEFT  = 7;   // leftmost (outer)

// Max sensors a plain line can cover. With 8 sensors a normal line usually
// covers 2–3. Anything above this (but below ALL_ON_MIN) is treated as a node.
constexpr uint8_t LINE_MAX_ACTIVE = 3;
// Sensors that must be on for T / cross / end-box. Drop to 7 if one sensor is flaky.
constexpr uint8_t ALL_ON_MIN = 8;

// Calibration samples per sensor. 3750 x 8 reads keeps the original ~3.4 s spin.
constexpr int CAL_SAMPLES = 3750;

// ---------------- EEPROM layout ----------------
constexpr int EE_MID  = 0;                    // 8 bytes
constexpr int EE_MAX  = EE_MID + SENSOR_COUNT; // 8 bytes
constexpr int EE_MIN  = EE_MAX + SENSOR_COUNT; // 8 bytes
constexpr int EE_PATH = 30;                   // maze path, terminated by 5
constexpr uint8_t PATH_MAX = 50;

static_assert(EE_MIN + SENSOR_COUNT <= EE_PATH, "Calibration overlaps saved path in EEPROM");
