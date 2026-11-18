#include <Arduino.h>
#include "globals.h"

// Mode 7 — "Arduino Bluetooth RC Car" style commands over Serial (HC-05 on D0/D1)
void bluetooth() {
  int spd = 10;
  while (true) {
    if (Serial.available() > 0) {
      char x = Serial.read();
      Serial.println(x);

      if      (x == 'F') motor(spd * spl, spd * spr);
      else if (x == 'B') motor(-spd * spl, -spd * spr);
      else if (x == 'L') motor(-spd * spl, spd * spr);
      else if (x == 'R') motor(spd * spl, -spd * spr);
      else if (x == 'G') motor(0, spd * spr);
      else if (x == 'I') motor(spd * spl, 0);
      else if (x == 'H') motor(0, -spd * spr);
      else if (x == 'J') motor(-spd * spl, 0);
      else if (x == 'S') motor(0, 0);
      else if (x == 'U') digitalWrite(light, HIGH);
      else if (x == 'u') digitalWrite(light, LOW);
      else if (x >= '0' && x <= '9') spd = x - '0';
      else if (x == 'q') spd = 10;
    }
  }
}
