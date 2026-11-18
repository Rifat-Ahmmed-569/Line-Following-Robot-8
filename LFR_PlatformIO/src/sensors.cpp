#include <Arduino.h>
#include <EEPROM.h>
#include "globals.h"

// A6/A7 on the Nano are analog-only — fine, we only analogRead.
static const uint8_t SENSOR_PINS[SENSOR_COUNT] = {A0, A1, A2, A3, A4, A5, A6, A7};

static inline uint8_t sensor_pin(uint8_t i) {
  return SENSOR_A0_IS_LEFT ? SENSOR_PINS[SENSOR_COUNT - 1 - i] : SENSOR_PINS[i];
}

// Reads all 8 sensors, thresholds against calibrated mid-points.
// sensor = bit pattern (bit i = s[i]), sum = sensors on line.
void reading() {
  sensor = 0;
  sum = 0;
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    s[i] = (analogRead(sensor_pin(i)) > mid[i]) ? 1 : 0;
    sensor |= s[i] << i;
    sum += s[i];
  }
}

bool center_on() {
  return s[S_CR] || s[S_CL];
}

int inner_sum() {
  int t = 0;
  for (uint8_t i = 1; i < SENSOR_COUNT - 1; i++) t += s[i];
  return t;
}

// Spin over the line and record min/max per sensor; mid = threshold.
void cal() {
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    maximum[i] = 0;
    minimum[i] = 1023;
  }
  motor(80, -80);
  for (int n = 0; n < CAL_SAMPLES; n++) {
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
      int v = analogRead(sensor_pin(i));
      maximum[i] = max(maximum[i], v);
      minimum[i] = min(minimum[i], v);
    }
  }
  motor(0, 0);
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    mid[i] = (maximum[i] + minimum[i]) / 2;
    EEPROM.update(EE_MID + i, mid[i] / 4);
    EEPROM.update(EE_MAX + i, maximum[i] / 4);
    EEPROM.update(EE_MIN + i, minimum[i] / 4);
    Serial.print(maximum[i]); Serial.print(' ');
    Serial.print(mid[i]);     Serial.print(' ');
    Serial.println(minimum[i]);
  }
}

// Raw values, printed LEFT -> RIGHT (s7 ... s0)
void analog_reading() {
  while (true) {
    for (int i = SENSOR_COUNT - 1; i >= 0; i--) {
      Serial.print(analogRead(sensor_pin(i)));
      Serial.print(' ');
    }
    Serial.println();
  }
}

// Thresholded values, printed LEFT -> RIGHT, then the decimal pattern
void digital_reading() {
  while (true) {
    reading();
    for (int i = SENSOR_COUNT - 1; i >= 0; i--) {
      Serial.print(s[i]);
      Serial.print(' ');
    }
    Serial.println(sensor);
  }
}
