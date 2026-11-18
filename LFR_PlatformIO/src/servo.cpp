#include <Arduino.h>
#include "globals.h"

// Gripper hooks for path codes 6 (grab) and 7 (release).
// Add Servo code here; keep the counter++ so the path skips the action step.
void grab() {
  counter++;
}

void release() {
  counter++;
}
