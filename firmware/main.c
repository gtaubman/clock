/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#include "button.h"
#include "i2cmaster.h"
#include "rtc.h"

// Ports for serial clock, data, and latch.
#define SDA_PORT PORTD
#define CLK_PORT PORTD
#define SDL_PORT PORTD

// Pins for serial clock, data, and latch.
#define SDA_PIN 0
#define CLK_PIN 1
#define SDL_PIN 2

#define RTC_INTERRUPT_PIN 3

#define HOUR_BTN_PIN 4
#define MIN_BTN_PIN 5
#define SEC_BTN_PIN 6

typedef struct digit {
  unsigned int a : 1;
  unsigned int b : 1;
  unsigned int c : 1;
  unsigned int d : 1;
  unsigned int e : 1;
  unsigned int f : 1;
  unsigned int g : 1;
  unsigned int dp : 1;
} Digit;

// Digit segment configuration for each digit.
Digit zero =  {1, 1, 1, 1, 1, 1, 0, 0};
Digit one =   {0, 1, 1, 0, 0, 0, 0, 0};
Digit two =   {1, 1, 0, 1, 1, 0, 1, 0};
Digit three = {1, 1, 1, 1, 0, 0, 1, 0};
Digit four =  {0, 1, 1, 0, 0, 1, 1, 0};
Digit five =  {1, 0, 1, 1, 0, 1, 1, 0};
Digit six =   {0, 0, 1, 1, 1, 1, 1, 0};
Digit seven = {1, 1, 1, 0, 0, 0, 0, 0};
Digit eight = {1, 1, 1, 1, 1, 1, 1, 0};
Digit nine =  {1, 1, 1, 0, 0, 1, 1, 0};
Digit clear = {0, 0, 0, 0, 0, 0, 0, 0};

// A lookup table from actual integer to 7-segment display integer.
Digit* digits[11] = {
  &zero,
  &one,
  &two,
  &three,
  &four,
  &five,
  &six,
  &seven,
  &eight,
  &nine,
  &clear,
};

volatile uint8_t update_time = 0;

volatile int time[6] = {0, 0, 0, 0, 0, 0};

Button hour_button;
Button min_button;
Button sec_button;

#define CLOCK_LOW() (CLK_PORT &= ~(1 << CLK_PIN))
#define CLOCK_HIGH() (CLK_PORT |= 1 << CLK_PIN)

#define DATA_LOW() (SDA_PORT &= ~(1 << SDA_PIN))
#define DATA_HIGH() (SDA_PORT |= 1 << SDA_PIN)

#define LATCH_LOW() (SDL_PORT &= ~(1 << SDL_PIN))
#define LATCH_HIGH() (SDL_PORT |= 1 << SDL_PIN)

void set_digit(int digit_number, int digit_value) {
  // Set the clock low to start with.
  CLOCK_LOW();

  // Also make sure we're not latched.
  LATCH_LOW();

  // Shift out the digit value data first.
  for (int i = 7; i >= 0; --i) {
    // Reinterpret our digit struct as an int to pull out each segment's bit.
    (*((uint8_t*) digits[digit_value]) & (1 << i)) ? DATA_HIGH() : DATA_LOW();
    CLOCK_HIGH();
    CLOCK_LOW();
  }

  // Shift out the digit selector.
  for (int i = 7; i >= 0; --i) {
    digit_value < 0 ? DATA_LOW() : (i == digit_number) ? DATA_HIGH() : DATA_LOW();
    CLOCK_HIGH();
    CLOCK_LOW();
  }

  LATCH_HIGH();
  LATCH_LOW();
}

inline void set_time() {
  uint8_t sec = 0;
  if (rtc_read_seconds(&sec) != 0) {
    time[4] = time[5] = 9;
  } else {
    time[4] = sec / 10;
    time[5] = sec % 10;
  }
  if (rtc_read_minutes(&sec) != 0) {
    time[2] = time[3] = 9;
  } else {
    time[2] = sec / 10;
    time[3] = sec % 10;
  }
  if (rtc_read_hours(&sec) != 0) {
    time[0] = time[1] = 9;
  } else {
    time[0] = sec / 10;
    time[1] = sec % 10;

    // If the hours tens place is zero, don't show it at all.  We don't want
    // it to be 08:00:00, just 8:00:00.  However, if both hour digits are
    // zero, then we want to show them both.
    if (time[0] == 0 && time[1] != 0) {
      time[0] = -1;
    }
  }

#ifdef TEMP_INSTEAD_OF_SECS
  if (rtc_read_temp_c(&sec) != 0) {
    time[4] = time[5] = 9;
  } else {
    sec = 32 + (sec * 9.0 / 5.0);
    time[4] = sec / 10;
    time[5] = sec % 10;
  }
#endif
}

int main(void) {
  // Make the serial control ports all output pins.
  DDRD |= 1 << SDA_PIN;
  DDRD |= 1 << CLK_PIN;
  DDRD |= 1 << SDL_PIN;

  // Set up the hour, minute, and seconds button pins as inputs with the pullup
  // enabled.
  DDRD &= ~(1 << HOUR_BTN_PIN);
  PORTD |= (1 << HOUR_BTN_PIN);

  DDRD &= ~(1 << MIN_BTN_PIN);
  PORTD |= (1 << MIN_BTN_PIN);

  DDRD &= ~(1 << SEC_BTN_PIN);
  PORTD |= (1 << SEC_BTN_PIN);

  // Enable interrupts for both rising and falling edges on the RTC interrupt
  // pin.
  // First we mark the pin as an input pin.
  DDRD &= ~(1 << RTC_INTERRUPT_PIN);

  // Next we enable the internal pullup.
  PORTD |= (1 << RTC_INTERRUPT_PIN);

  // The DS3231 has a falling edge of its 1 Hz square wave lined up with second
  // changes, so we only trigger on falling edges.
  EICRA |= (1 << ISC11);

  // Finally, enable INT1.
  EIMSK |= (1 << INT1);

  // Initialize our i2c interface to talk to the DS3231.
  i2c_init();

  sei();

#ifdef SET_TIME
  rtc_set_time(10, 17, 5);
#endif

  if (rtc_enable_square_wave() != 0) {
    return -1;
  }

  while (1) {
    if (update_time) {
      update_time = 0;

      // Because set_time() takes time to fetch the time from the DS3231, the
      // last digit in the cycle would be left on for extra time (the normal
      // amount of time plus however long it takes to fetch the time from the
      // DS3231).  This would result it pulsing brighter.  To keep it the same
      // brightness regardless of whether or not we fetch the time, we disable
      // it before fetching the time.
      set_digit(5, 10);

      set_time();
    }

    set_button(&hour_button, (PIND & (1 << HOUR_BTN_PIN)) == 0);
    if (is_button_set(&hour_button)) {
      // They hit the hours button.
      rtc_inc_hours();
      set_time();
    }

    set_button(&min_button, (PIND & (1 << MIN_BTN_PIN)) == 0);
    if (is_button_set(&min_button)) {
      // They hit the mins button.
      rtc_inc_mins();
      set_time();
    }

    set_button(&sec_button, (PIND & (1 << SEC_BTN_PIN)) == 0);
    if (is_button_set(&sec_button)) {
      // They hit the secs button.
      rtc_inc_secs();
      set_time();
    }

    // Loop through and display all the digits.
    for (int i = 0; i < 6; ++i) {
      set_digit(i, time[i]);
    }

  }

  return 0;
}

ISR(INT1_vect) {
  update_time = 1;
}
