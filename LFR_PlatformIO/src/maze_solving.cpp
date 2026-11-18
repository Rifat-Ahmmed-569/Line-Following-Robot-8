#include <Arduino.h>
#include "globals.h"

static void advance(bool mode) {
  if (mode) counter--; else counter++;
  digitalWrite(light, HIGH);
  if (path[counter] == 6) grab();
  else if (path[counter] == 7) release();
}

// Mode 10 (mode=0): run the shortest path from start.
// Mode 5  (mode=1): run it backwards from the end point.
void maze_solving(bool mode) {
  // Fresh boot: path[] is empty in RAM -> load the last scan from EEPROM
  if (path[0] == 0 && !load_path()) {
    Serial.println(F("No saved path - run a maze scan first"));
    return;
  }

  counter = 0;
  flag = 's'; cross = 's'; pos = 0;
  if (mode) {
    // Reverse run: swap L/R and start from the last real step
    while (counter < PATH_MAX && path[counter] != 5) {
      if (path[counter] == 3) path[counter] = 1;
      else if (path[counter] == 1) path[counter] = 3;
      counter++;
    }
    if (counter > 0) counter--;   // don't start on the '5' end marker
  }

  while (true) {
    reading();

    if (sum == 0) {
      if (flag != 's') {
        if (flag == 'l') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
        while (!center_on()) reading();
        flag = 's'; cross = 's'; pos = 0;
      }
      else if (pos > -3 && pos < 3) {
        m2 = millis();
        while (sum == 0) {
          reading();
          if (millis() - m2 > u_turn_delay) {
            brake(); digitalWrite(light, HIGH);
            motor(0, 0);
            if (path[counter] == 6) grab();
            else if (path[counter] == 7) release();
            if (side == 'l') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
            while (!center_on()) reading();
            if (side == 'r') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
            delay(turn_brake);
            digitalWrite(light, LOW);
            pos = 0;
            break;
          }
        }
      }
    }

    else if (sum <= LINE_MAX_ACTIVE) {
      if (cross != 's') {
        if (cross == 'l') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
        while (inner_sum() > 0) reading();
        while (!center_on()) reading();
        cross = 's'; flag = 's'; pos = 0;
      }
      track_line(PROFILE_MAZE);
    }

    else if (sum < ALL_ON_MIN) {
      if (s[S_LEFT] && !s[S_RIGHT] && center_on()) {
        flag = 'l';
        while (s[S_LEFT] && !s[S_RIGHT]) reading();
        if (!s[S_RIGHT]) {
          delay(node_delay);
          reading();
          if (sum != 0) {
            if (path[counter] == 1) cross = 'l';
            advance(mode);
          }
        }
      }
      else if (s[S_RIGHT] && !s[S_LEFT] && center_on()) {
        flag = 'r';
        while (!s[S_LEFT] && s[S_RIGHT]) reading();
        if (!s[S_LEFT]) {
          delay(node_delay);
          reading();
          if (sum != 0) {
            if (path[counter] == 3) cross = 'r';
            advance(mode);
          }
        }
      }
      m1 = millis();
    }

    else {
      flag = (path[counter] == 1) ? 'l' : 'r';
      m2 = millis();
      while (s[S_LEFT] || s[S_RIGHT]) {
        reading();
        if (millis() - m2 > stop_timer) {
          motor(0, 0);
          return;
        }
      }
      delay(node_delay);
      reading();
      if (sum != 0 && path[counter] != 2) cross = (path[counter] == 1) ? 'l' : 'r';
      advance(mode);
      m1 = millis();
    }

    if (millis() - m1 > 500) {
      flag = 's';
      digitalWrite(light, LOW);
    }
  }
}
