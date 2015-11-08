#ifndef _RTC_H_
#define _RTC_H_

#include <avr/io.h>


uint8_t rtc_read_seconds(uint8_t* seconds);
uint8_t rtc_read_minutes(uint8_t* minutes);
uint8_t rtc_read_hours(uint8_t* hours);

uint8_t rtc_read_temp_c(uint8_t* temp_c);

uint8_t rtc_set_time(uint8_t h, uint8_t m, uint8_t s);

uint8_t rtc_enable_square_wave();
void rtc_disable_square_wave();

uint8_t rtc_read_reg(uint8_t reg, uint8_t* val);

uint8_t rtc_inc_hours();
uint8_t rtc_inc_mins();
uint8_t rtc_inc_secs();

uint8_t rtc_set_secs(uint8_t secs);

#endif
