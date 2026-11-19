// LFR / maze solver — original by Rifat Ahmmed
// PlatformIO port, 8-sensor array on A0..A7 (Arduino Nano)

#include <Arduino.h>
#include <EEPROM.h>
#include "globals.h"

// ---------------- Global state ----------------
char side  = 'l';  // left or right side of the line (for line following) - Feff hand rule or Right hand rule?
char flag  = 's';
char cross = 's';
int  pos   = 0;
byte path[PATH_MAX];
byte counter = 0;
uint32_t m1 = 0, m2 = 0;

int s[SENSOR_COUNT];
int mid[SENSOR_COUNT], maximum[SENSOR_COUNT], minimum[SENSOR_COUNT];
int sensor = 0;
int sum    = 0;

void setup() {
  pinMode(left_motor_forward, OUTPUT);
  pinMode(left_motor_backward, OUTPUT);
  pinMode(left_motor_speed, OUTPUT);
  pinMode(right_motor_forward, OUTPUT);
  pinMode(right_motor_backward, OUTPUT);
  pinMode(right_motor_speed, OUTPUT);
  pinMode(light, OUTPUT);
  pinMode(sw, INPUT_PULLUP);
  Serial.begin(9600);

  bool calibrated = false;
  Serial.println(F("max mid min  (s0=right ... s7=left)"));
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    mid[i]     = EEPROM.read(EE_MID + i) * 4;
    maximum[i] = EEPROM.read(EE_MAX + i) * 4;
    minimum[i] = EEPROM.read(EE_MIN + i) * 4;
    if (mid[i] != 255 * 4) calibrated = true;
    Serial.print(maximum[i]); Serial.print(' ');
    Serial.print(mid[i]);     Serial.print(' ');
    Serial.println(minimum[i]);
  }
  if (!calibrated) Serial.println(F("!! Not calibrated - press button 2x on the line"));
}

// Button press count -> mode
//  1  line follow
//  2  calibrate (robot spins in place over the line)
//  3  straight-line motor test
//  4  digital sensor reading  (serial)
//  5  solve maze from end point
//  6  analog sensor reading   (serial)  [was unreachable as a 2nd "2" before]
//  7  bluetooth control
//  8  scan maze, then solve back from end point
//  9  scan maze, stop at end point
// 10  solve maze from start
// 11  print saved path        (serial)
void loop() {
  int r = button_read();
  if      (r == 1)  line_follow();
  else if (r == 2)  cal();
  else if (r == 3)  motor(10 * spl, 10 * spr); // Straight Line Motor Test !!!!! SPL = Left Motor Speed SPR = Right Motor Speed
  else if (r == 4)  digital_reading();
  else if (r == 5)  maze_solving(1);
  else if (r == 6)  analog_reading();
  else if (r == 7)  bluetooth();
  else if (r == 8)  maze_scan(1);
  else if (r == 9)  maze_scan(0);
  else if (r == 10) maze_solving(0);
  else if (r == 11) print_saved_path();
}
