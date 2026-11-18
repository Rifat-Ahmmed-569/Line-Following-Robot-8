#include <Arduino.h>
#include "globals.h"

// Mode 1 — fast line follow with 90° turns, crosses and dead-end U-turns.
void line_follow() {
  while (true) {
    reading();

    if (sum == 0) {
      if (flag != 's') {
        // 90° turn that was flagged at a node
        brake();   // original had `brake;` (no parentheses = never ran)
        if (flag == 'l') motor(-9 * spl, 9 * spr); else motor(9 * spl, -9 * spr);
        while (!center_on()) reading();
        if (flag == 'r') motor(-9 * spl, 9 * spr); else motor(9 * spl, -9 * spr);
        delay(turn_brake);
        flag = 's'; cross = 's'; pos = 0;
      }
      else if (pos > -3 && pos < 3) {
        // Line lost near centre -> dead end -> U-turn
        m2 = millis();
        while (sum == 0) {
          reading();
          if (millis() - m2 > u_turn_delay) {
            Ubrake(); digitalWrite(light, HIGH);
            if (side == 'l') motor(-9 * spl, 9 * spr); else motor(9 * spl, -9 * spr);
            while (!center_on()) reading();
            pos = 0;
            break;
          }
        }
      }
    }

    else if (sum <= LINE_MAX_ACTIVE) {
      // Plain line (1..3 sensors)
      if (cross != 's') {
        brake();
        if (cross == 'l') motor(-9 * spl, 9 * spr); else motor(9 * spl, -9 * spr);
        while (inner_sum() > 0) reading();
        while (!center_on()) reading();
        if (flag == 'r') motor(-9 * spl, 9 * spr); else motor(9 * spl, -9 * spr);
        delay(turn_brake);
        cross = 's'; flag = 's'; pos = 0;
      }
      track_line(PROFILE_FAST);
    }

    else if (sum < ALL_ON_MIN) {
      // Node: one outer sensor + centre
      if (s[S_LEFT] && !s[S_RIGHT] && center_on()) {          // left branch
        flag = 'l';
        while (s[S_LEFT] && !s[S_RIGHT]) reading();
        if (!s[S_RIGHT]) {
          delay(node_delay);
          reading();
          if (sum != 0 && side == 'l') cross = 'l';
        }
      }
      else if (s[S_RIGHT] && !s[S_LEFT] && center_on()) {     // right branch
        flag = 'r';
        while (!s[S_LEFT] && s[S_RIGHT]) reading();
        if (!s[S_LEFT]) {
          delay(node_delay);
          reading();
          if (sum != 0 && side == 'r') cross = 'r';
        }
      }
      m1 = millis();
    }

    else {
      // All sensors on: T, cross, or end box
      flag = side;
      m2 = millis();
      bool parked = false;
      while (s[S_LEFT] || s[S_RIGHT]) {
        reading();
        if (millis() - m2 > stop_timer) {
          // End box: stop and wait until lifted off
          brake();
          motor(0, 0);
          while (sum >= ALL_ON_MIN) reading();
          parked = true;
          break;
        }
      }
      if (parked) continue;
      delay(node_delay);
      reading();
      if (sum != 0) cross = side;
      m1 = millis();
    }

    if (millis() - m1 > 500) flag = 's';
  }
}
