#ifndef _BTN_H_
#define _BTN_H_

#include <avr/io.h>

// Button provides 8-step button debouncing, as well as repeat trigger
// protection.  Every time a button value is read from a pin,
// ingest_button_value() should be called with the read value.  After ingesting
// the button value, the is_button_*() functions may be called to query the
// debounced button state.
typedef struct Button {
  uint8_t state;
} Button;

// ingest_button_value accepts the current button value.  This should be called
// on every read of a button.
void ingest_button_value(Button* b, uint8_t value);

// is_button_pushed returns true if the button has been pressed down with
// sufficient debouncing.  Will only trigger once per button press.
uint8_t is_button_pushed(Button* b);

// is_button_lifted returns true if the button has been prssed and lifted again
// with sufficient debouncing.  Will only trigger once per button press.
uint8_t is_button_lifted(Button* b);

#endif
