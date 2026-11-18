#pragma once
#include "config.h"

// ---------------- Shared state (defined in main.cpp) ----------------
extern char side;     // 'l' = left-hand rule, 'r' = right-hand rule
extern char flag;
extern char cross;
extern int  pos;
extern byte path[PATH_MAX];
extern byte counter;
extern uint32_t m1, m2;

extern int s[SENSOR_COUNT];        // digital state per sensor (1 = on line)
extern int mid[SENSOR_COUNT];
extern int maximum[SENSOR_COUNT];
extern int minimum[SENSOR_COUNT];
extern int sensor;                 // bit pattern, bit i = s[i]
extern int sum;                    // number of sensors on line

// ---------------- Line tracking profiles ----------------
enum LineProfile : uint8_t { PROFILE_FAST, PROFILE_MAZE };

// ---------------- Prototypes ----------------
// motor.cpp
void motor(int a, int b);
void brake();
void Ubrake();
void rotate_backwards();

// sensors.cpp
void reading();
bool center_on();      // either centre sensor on line
int  inner_sum();      // sensors 1..6 (all except the two outer ones)
void cal();
void analog_reading();
void digital_reading();

// line_tracking.cpp
void track_line(LineProfile profile);

// button.cpp
int button_read();

// modes
void line_follow();
void maze_scan(bool mode);
void maze_solving(bool mode);
void bluetooth();

// maze helpers
void algorithm();
void recording();
bool load_path();
void print_saved_path();

// servo.cpp
void grab();
void release();
