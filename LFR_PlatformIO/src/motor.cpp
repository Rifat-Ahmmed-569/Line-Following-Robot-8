#include <Arduino.h>
#include "globals.h"

// a = left, b = right. Negative = reverse. Clamped to 0..255 PWM.
void motor(int a, int b) {
  if (a >= 0) {
    digitalWrite(left_motor_forward, HIGH);
    digitalWrite(left_motor_backward, LOW);
  } else {
    a = -a;
    digitalWrite(left_motor_forward, LOW);
    digitalWrite(left_motor_backward, HIGH);
  }
  if (b >= 0) {
    digitalWrite(right_motor_forward, HIGH);
    digitalWrite(right_motor_backward, LOW);
  } else {
    b = -b;
    digitalWrite(right_motor_forward, LOW);
    digitalWrite(right_motor_backward, HIGH);
  }
  analogWrite(left_motor_speed, constrain(a, 0, 255));
  analogWrite(right_motor_speed, constrain(b, 0, 255));
}

void rotate_backwards() {
  // TODO: 180° turn before solving back from the end point
}

void brake() {
  motor(-250, -250); delay(brake_time);
  motor(0, 0);       delay(100);
}

void Ubrake() {
  motor(-250, -250); delay(Ubrake_time);
  motor(0, 0);       delay(100);
}
