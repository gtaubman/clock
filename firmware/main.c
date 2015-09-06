/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#include "i2cmaster.h"
#include "rtc.h"

// TODO(gtaubman): Get rid of this and decrement the pin numbers for SDA, CLK,
// and SDL.
#define LED_PIN 0

// Ports for serial clock, data, and latch.
#define SDA_PORT PORTD
#define CLK_PORT PORTD
#define SDL_PORT PORTD

// Pins for serial clock, data, and latch.
#define SDA_PIN 1
#define CLK_PIN 2
#define SDL_PIN 3

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
Digit* digits[10] = {
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
};

volatile int time[6] = {0, 0, 0, 0, 0, 0};

void delay_ms(uint16_t count) {
  while (count--) {
    _delay_ms(1);
  }
}

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

int main(void) {
  // TODO(gtaubman): Remove.
  DDRD |= 1 << LED_PIN;  // Make the LED an output pin.

  // Make the serial control ports all output pins.
  DDRD |= 1 << SDA_PIN;
  DDRD |= 1 << CLK_PIN;
  DDRD |= 1 << SDL_PIN;

  // Initialize our i2c interface to talk to the DS3231.
  i2c_init();

#ifdef SET_TIME
  rtc_set_time(10, 17, 5);
#endif

  int count = 0;
  uint8_t sec = 0;
  while (1) {
    if (++count == 100) {
      count = 0;
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
        // it to be 08:00:00, just 8:00:00.
        if (time[0] == 0) {
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

    // Loop through and display all the digits.
    for (int i = 0; i < 6; ++i) {
      set_digit(i, time[i]);
    }

  }

  return 0;
}
