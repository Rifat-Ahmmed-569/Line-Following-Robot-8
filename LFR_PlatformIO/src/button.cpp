#include <Arduino.h>
#include "globals.h"

// Counts button presses (>15 ms each). Returns once there's a 1 s gap.
int button_read() {
  int cl = 0;
  while (true) {
    if (digitalRead(sw) != LOW) return cl;

    int t = 0;
    digitalWrite(light, HIGH);
    while (digitalRead(sw) == LOW) { delay(1); t++; }
    digitalWrite(light, LOW);
    if (t <= 15) return cl;   // bounce, ignore

    cl++;
    t = 0;
    while (digitalRead(sw) == HIGH) {
      delay(1);
      if (++t > 1000) return cl;
    }
  }
}
