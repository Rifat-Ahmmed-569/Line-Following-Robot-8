#include <Arduino.h>
#include <EEPROM.h>
#include "globals.h"

// Save path (up to and including the 5 end marker) to EEPROM
void recording() {
  for (uint8_t i = 0; i < PATH_MAX; i++) {
    EEPROM.update(EE_PATH + i, path[i]);
    if (path[i] == 5) return;
  }
}

// Load path from EEPROM. Returns false if nothing valid is stored.
bool load_path() {
  for (uint8_t i = 0; i < PATH_MAX; i++) {
    byte v = EEPROM.read(EE_PATH + i);
    if (v < 1 || v > 7) break;
    path[i] = v;
    if (v == 5) return true;
  }
  path[0] = 0;
  return false;
}

void print_saved_path() {
  for (uint8_t i = 0; i < PATH_MAX; i++) {
    byte v = EEPROM.read(EE_PATH + i);
    Serial.println(v);
    if (v == 5 || v == 0 || v == 255) break;
  }
}
