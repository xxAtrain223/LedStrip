/*
 * Project: Lightweight millisecond tracking library
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/millisecond-tracking-library-for-avr/
 */

#ifndef MILLIS_H_
#define MILLIS_H_

/* 
The millis() function known from Arduino
Calling millis() will return the milliseconds since the program started
Tested on atmega328p
Using content from http://www.adnbr.co.uk/articles/counting-milliseconds
Author: Monoclecat, https://github.com/monoclecat/avr-millis-function
REMEMBER: Add sei(); after init_millis() to enable global interrupts!
 */

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

extern volatile unsigned long timer3_millis;
//NOTE: A unsigned long holds values from 0 to 4,294,967,295 (2^32 - 1). It will roll over to 0 after reaching its maximum value.

ISR(TIMER3_COMPA_vect);

void init_millis(unsigned long f_cpu);

unsigned long millis();

#endif /* MILLIS_H_ */
