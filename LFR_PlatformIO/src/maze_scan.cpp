#include <Arduino.h>
#include "globals.h"

// Path codes: 1 = left, 2 = straight, 3 = right, 4 = U-turn, 5 = end,
//             6 = grab, 7 = release

static void push_step(byte v) {
  if (counter < PATH_MAX - 1) path[counter++] = v;   // keep last slot for the 5
}

// Mode 9 (mode=0): scan and stop at the end.
// Mode 8 (mode=1): scan, then solve back from the end.
void maze_scan(bool mode) {
  counter = 0;
  flag = 's'; cross = 's'; pos = 0;

  while (true) {
    reading();

    if (sum == 0) {
      if (flag != 's') {
        brake();
        if (flag == 'l') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
        while (!center_on()) reading();
        if (flag == 'r') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
        delay(turn_brake);
        flag = 's'; cross = 's'; pos = 0;
      }
      else if (pos > -3 && pos < 3) {
        m2 = millis();
        while (sum == 0) {
          reading();
          if (millis() - m2 > u_turn_delay) {
            brake(); digitalWrite(light, HIGH);
            if (side == 'l') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
            while (!center_on()) reading();
            if (side == 'r') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
            delay(turn_brake);
            push_step(4);
            digitalWrite(light, LOW);
            pos = 0;
            break;
          }
        }
      }
    }

    else if (sum <= LINE_MAX_ACTIVE) {
      if (cross != 's') {
        brake();
        if (cross == 'l') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
        while (inner_sum() > 0) reading();
        while (!center_on()) reading();
        if (flag == 'r') motor(-6 * spl, 6 * spr); else motor(6 * spl, -6 * spr);
        delay(turn_brake);
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
            if (side == 'l') cross = 'l';
            push_step(side == 'l' ? 1 : 2);
            digitalWrite(light, HIGH);
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
            if (side == 'r') cross = 'r';
            push_step(side == 'r' ? 3 : 2);
            digitalWrite(light, HIGH);
          }
        }
      }
      m1 = millis();
    }

    else {
      flag = side;
      m2 = millis();
      while (s[S_LEFT] || s[S_RIGHT]) {
        reading();
        if (millis() - m2 > stop_timer) {
          // End point reached
          motor(0, 0);
          path[counter] = 5;
          counter++;
          algorithm();
          recording();
          if (mode) {
            rotate_backwards();   // 180° (TODO in motor.cpp)
            maze_solving(1);
          }
          return;
        }
      }
      delay(node_delay);
      reading();
      if (sum != 0) cross = side;
      push_step(side == 'l' ? 1 : 3);
      digitalWrite(light, HIGH);
      m1 = millis();
    }

    if (millis() - m1 > 500) {
      flag = 's';
      digitalWrite(light, LOW);
    }
  }
}
