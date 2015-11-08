#include "button.h"

void set_button(Button* b, uint8_t value) {
  // Push a new button value onto the stack.
  b->state = (b->state << 1) | (value & 1);
}

uint8_t is_button_set(Button* b) {
  // If all 7 button history bits are on, but the button-set flag is off, the
  // button has been pressed!  Set the button-set flag and return that the
  // button has been set.
  if (b->state == 0x7F) {
    return 1;
  }

  return 0;
}
