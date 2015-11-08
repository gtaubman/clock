#include "button.h"

void set_button(Button* b, uint8_t value) {
  b->state = (b->state << 1) | (value & 1);
}

uint8_t is_button_set(Button* b) {
  return b->state == 0x7F;
}

uint8_t is_button_lifted(Button* b) {
  return b->state == 0xFE;
}
