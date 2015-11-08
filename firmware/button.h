#ifndef _BTN_H_
#define _BTN_H_

#include <avr/io.h>

typedef struct Button {
  uint8_t state;
} Button;

void set_button(Button* b, uint8_t value);
uint8_t is_button_set(Button* b);

#endif
