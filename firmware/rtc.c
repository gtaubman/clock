#include "rtc.h"
#include "i2cmaster.h"

#define RTC_ADDRESS 0xD0


#define SECONDS_ADDRESS 0x00
#define MINUTES_ADDRESS 0x01
#define HOURS_ADDRESS 0x02
#define DAY_ADDRESS 0x03
#define DATE_ADDRESS 0x04

#define CONTROL_ADDRESS 0x0E

uint8_t dec2bcd(uint8_t d) {
    return ((d/10 * 16) + (d % 10));
}

uint8_t bcd2dec(uint8_t b) {
    return ((b/16 * 10) + (b % 16));
}

uint8_t rtc_read_reg(uint8_t reg, uint8_t* val) {
  // Start off with a write.
  if (i2c_start(RTC_ADDRESS) != 0) {
    return 1;
  }

  // Indicate which register we'd like to read.
  if (i2c_write(reg) != 0) {
    return 2;
  }

  // Start again, indicating a read this time.
  if (i2c_rep_start(RTC_ADDRESS | 1) != 0) {
    return 3;
  }

  // Read one byte, and nack it because we're done.
  *val = i2c_readNak();

  // Done!
  i2c_stop();

  return 0;
}

uint8_t rtc_read_bcd_reg(uint8_t reg, uint8_t* val) {
  // Start off with a write.
  if (i2c_start(RTC_ADDRESS) != 0) {
    return 1;
  }

  // Indicate which register we'd like to read.
  if (i2c_write(reg) != 0) {
    return 2;
  }

  // Start again, indicating a read this time.
  if (i2c_rep_start(RTC_ADDRESS | 1) != 0) {
    return 3;
  }

  // Read one byte, and nack it because we're done.
  unsigned char r = i2c_readNak();
  *val = bcd2dec(r);

  // Done!
  i2c_stop();

  return 0;
}

uint8_t rtc_write_reg(uint8_t reg, uint8_t val) {
  // Start off!
  if (i2c_start(RTC_ADDRESS) != 0) {
    return 1;
  }

  if (i2c_write(reg) != 0) {
    return 2;
  }

  if (i2c_write(val) != 0) {
    return 3;
  }

  // Done!
  i2c_stop();

  return 0;
}

uint8_t rtc_read_seconds(uint8_t* seconds) {
  return rtc_read_bcd_reg(SECONDS_ADDRESS, seconds);
}

uint8_t rtc_read_minutes(uint8_t* minutes) {
  return rtc_read_bcd_reg(MINUTES_ADDRESS, minutes);
}

uint8_t rtc_read_hours(uint8_t* hours) {
  return rtc_read_bcd_reg(HOURS_ADDRESS, hours);
}

uint8_t rtc_read_temp_c(uint8_t* temp_c) {
  return rtc_read_reg(0x11, temp_c);
}

uint8_t rtc_set_time(uint8_t h, uint8_t m, uint8_t s) {
  // Start off!
  if (i2c_start(RTC_ADDRESS) != 0) {
    return 1;
  }

  // Start off writing seconds, and then continue with minutes and hours.
  if (i2c_write(SECONDS_ADDRESS) != 0) {
    return 2;
  }

  if (i2c_write(dec2bcd(s)) != 0) {
    return 3;
  }
  if (i2c_write(dec2bcd(m)) != 0) {
    return 4;
  }
  if (i2c_write(dec2bcd(h)) != 0) {
    return 5;
  }

  // Done!
  i2c_stop();

  return 0;
}

uint8_t rtc_enable_square_wave() {
  uint8_t control_register;
  if (rtc_read_reg(CONTROL_ADDRESS, &control_register) != 0) {
    return 1;
  }

  // Enable 1Hz square wave, and set the INTCN bit to 0 to enable square waves.
  control_register &= 0xE3;

  // Enable square wave output.
  control_register |= 0x40;

  if (i2c_start(RTC_ADDRESS) != 0) {
    return 2;
  }

  if (i2c_write(CONTROL_ADDRESS) != 0) {
    return 3;
  }

  if (i2c_write(control_register) != 0) {
    return 4;
  }

  i2c_stop();

  return 0;
}

void rtc_disable_square_wave() {
}

uint8_t rtc_inc_hours() {
  uint8_t hours;
  if (rtc_read_hours(&hours) != 0) {
    return 1;
  }

  if (rtc_write_reg(HOURS_ADDRESS, dec2bcd((hours + 1) % 24)) != 0) {
    return 2;
  }

  return 0;
}

uint8_t rtc_inc_mins() {
  uint8_t mins;
  if (rtc_read_minutes(&mins) != 0) {
    return 1;
  }

  if (rtc_write_reg(MINUTES_ADDRESS, dec2bcd((mins + 1) % 60)) != 0) {
    return 2;
  }

  return 0;
}

uint8_t rtc_inc_secs() {
  uint8_t secs;
  if (rtc_read_seconds(&secs) != 0) {
    return 1;
  }

  if (rtc_write_reg(SECONDS_ADDRESS, dec2bcd((secs + 1) % 60)) != 0) {
    return 2;
  }

  return 0;
}
