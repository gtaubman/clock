#include "button.h"

void ingest_button_value(Button* b, uint8_t value) {
  b->state = (b->state << 1) | (value & 1);
}

uint8_t is_button_pushed(Button* b) {
  return b->state == 0x7F;
}

uint8_t is_button_lifted(Button* b) {
  return b->state == 0xFE;
}
