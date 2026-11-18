#include <Arduino.h>
#include "globals.h"

// ============================================================================
//  LINE TRACKING — replaces the 6-sensor "if (sensor == 0b001100) ..." chains.
//
//  The active sensors must form one contiguous block of 1..LINE_MAX_ACTIVE.
//  err = 7 - (lo + hi)   -> -7 .. +7, in half-sensor steps from centre
//      err  0 : s3+s4 (dead centre)      err +7 : s0 only (far right)
//      err +1 : s3 only                  err -7 : s7 only (far left)
//  step = |err| indexes the tables below.
//
//  Speeds are in "units" (multiplied by spl / spr), same as the original.
//  Values were interpolated from the original 6-sensor tables — tune on track.
// ============================================================================

struct SpeedTable {
  int8_t straight;
  int8_t rOuter[8], rInner[8];   // line on RIGHT: left motor = outer, right = inner
  int8_t lOuter[8], lInner[8];   // line on LEFT:  right motor = outer, left = inner
};

//                         step: 0   1   2   3   4   5   6   7
static const SpeedTable FAST = {
  10,
  /* rOuter */            {10, 11, 11, 11, 11, 11, 11, 11},
  /* rInner */            {10,  9,  8,  7,  5,  2, -1, -4},
  /* lOuter */            {10, 11, 11, 11, 11, 11, 11, 11},
  /* lInner */            {10,  9,  8,  7,  5,  2, -1, -5},
};

static const SpeedTable MAZE = {
  10,
  /* rOuter */            {10, 10, 10, 10, 10, 10, 10, 10},
  /* rInner */            {10,  9,  8,  6,  3,  1, -1, -3},
  /* lOuter */            {10, 10, 10, 10, 10, 10, 10, 10},
  /* lInner */            {10,  9,  8,  6,  3,  1, -1, -3},
};

// pos memory per step (0 = don't touch). Outer two steps -> 3, 4, like the
// original, so the "lost line -> U-turn only if |pos| < 3" logic still holds.
static const int8_t POS_TABLE[8] = {0, 0, 1, 1, 2, 2, 3, 4};

void track_line(LineProfile profile) {
  const SpeedTable &t = (profile == PROFILE_FAST) ? FAST : MAZE;

  int8_t lo = -1, hi = -1;
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    if (s[i]) {
      if (lo < 0) lo = i;
      hi = i;
    }
  }
  // Nothing, or a split/noisy pattern: keep the last motor command
  // (the original also ignored patterns it didn't list).
  if (lo < 0 || (hi - lo + 1) != sum) return;

  int8_t err = 7 - (lo + hi);   // +ve = line toward s0 (right)

  if (err == 0) {
    // Anti-wobble: snap back toward centre proportional to how far out we were.
    // Don't let pos*5 exceed ~30 ms.
    if (pos != 0) {
      if (pos > 0) { motor(-10 * spl, 10 * spr); delay(pos * 5); }
      else         { motor(10 * spl, -10 * spr); delay(-pos * 5); }
      pos = 0;
    }
    motor(t.straight * spl, t.straight * spr);
    return;
  }

  uint8_t step = abs(err);
  int8_t p = POS_TABLE[step];

  if (err > 0) {                       // line on the right
    if (p && pos < p) pos = p;
    motor(t.rOuter[step] * spl, t.rInner[step] * spr);
  } else {                             // line on the left
    if (p && pos > -p) pos = -p;
    motor(t.lInner[step] * spl, t.lOuter[step] * spr);
  }
}
