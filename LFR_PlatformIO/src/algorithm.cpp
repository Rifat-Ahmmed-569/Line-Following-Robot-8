#include <Arduino.h>
#include "globals.h"

// Shortest-path reduction of the scanned path (removes U-turn loops).
// Works on left-hand-rule codes; right-hand runs are mirrored before/after.
void algorithm() {
  int w = 0, r = 0;   // w = write index, r = read index (were s / a)

  if (side == 'r') {
    while (r < PATH_MAX) {
      if (path[r] == 3) path[r] = 1;
      else if (path[r] == 1) path[r] = 3;
      else if (path[r] == 5) break;
      r++;
    }
  }

  r = 0;
  while (true) {
    if (path[r + 1] == 4) {
      r += 2;
      path[w] = path[w] + path[r];
    }
    if (path[w] == 4) {
      w--; r++;
      path[w] = path[w] + path[r];
    } else {
      w++; r++;
      path[w] = path[r];
    }
    if (path[w] == 5) break;
  }

  if (side == 'r') {
    r = 0;
    while (r < PATH_MAX) {
      if (path[r] == 3) path[r] = 1;
      else if (path[r] == 1) path[r] = 3;
      else if (path[r] == 5) break;
      r++;
    }
  }
}
