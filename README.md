# Digital LED Clock

![Pixel Clock](https://raw.githubusercontent.com/gtaubman/clock/master/images/pixel_clock.png)

This project contains KiCad board schematics, PCB layouts, and firmware for my
digital clock.

The clock features buttons to set the time, a hold button to aid in syncing the
clock to another time source, and a switch to toggle the right-most display
between showing seconds and the current temperature as reported by the real-time
clock.

![Clock In Case](https://raw.githubusercontent.com/gtaubman/clock/master/images/clock_case.jpg)

The clock is divided into two PCBs:

## The LED Board

![LED Board Populated](https://raw.githubusercontent.com/gtaubman/clock/master/images/led_board_populated.jpg)

Schematic:

![LED Board
Schematic](https://raw.githubusercontent.com/gtaubman/clock/master/images/led_schematic.png)

PCB Layout:

![LED Board
PCB](https://raw.githubusercontent.com/gtaubman/clock/master/images/led_pcb.png)

PCB fabricated by OSH Park:

![LED Board](https://raw.githubusercontent.com/gtaubman/clock/master/images/led_board_bare.jpg)

This board contains:

1.  Three 2-digit 7-segment displays (Lite-On LTG-5612AG).
1.  A TLC5916 constant-current LED driver to drive the LEDs.
1.  A 0.1 uF decoupling capacitor for the TLC5916.
1.  A 1000 Ohm resistor to set the current allowed through the TLC5916.
1.  6 power pins to drive the individual digits.
1.  3 serial data pins to drive the TLC5916 (serial data, serial clock, latch).
1.  2 pins for power and ground for the TLC5916.

## The Clock Board

![Clock Board Populated](https://raw.githubusercontent.com/gtaubman/clock/master/images/clock_board_populated.jpg)

Schematic:

![Clock Board Schematic](https://raw.githubusercontent.com/gtaubman/clock/master/images/clock_board_schematic.png)

PCB Layout:

![Clock Board PCB](https://raw.githubusercontent.com/gtaubman/clock/master/images/clock_board_pcb.png)

PCB fabricated by OSH Park:

![Clock Board](https://raw.githubusercontent.com/gtaubman/clock/master/images/clock_board_bare.jpg)

The clock board is responsible for driving the entire clock logic, as well as
interfacing with the real-time clock chip, and allowing user control over the
clock functionality.  It consists of:

1.  One Atmega 328pu running the show.
1.  One DS3231 real-time clock, communicating with the AVR over i2c.
1.  One 74HC595 shift register to control which digit is powered on.
1.  6 transistors to power on digits.
1.  6 1k, 2 4.7k, and 2 10k resistors.
1.  4 push buttons for setting the time and temperature display modes.
1.  3 decoupling capacitors.
1.  2 10uF capacitors and a 7805 voltage regulator for supplying power.
1.  Pins to send power, data, and chip power to the LED board.
1.  An optional LED to indicate when the board is powered on.
