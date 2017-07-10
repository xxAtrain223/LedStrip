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



volatile unsigned long timer2_millis; 
//NOTE: A unsigned long holds values from 0 to 4,294,967,295 (2^32 - 1). It will roll over to 0 after reaching its maximum value.

ISR(TIMER2_COMPA_vect)
{
  timer2_millis++;  
}

void init_millis(unsigned long f_cpu)
{
  unsigned long ctc_match_overflow;
  
  ctc_match_overflow = ((f_cpu / 1000) / 8); //when timer1 is this value, 1ms has passed
    
  // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
  TCCR2B |= (1 << WGM22) | (1 << CS21);
  
  // high byte first, then low byte
  OCR2AH = (ctc_match_overflow >> 8);
  OCR2AL = ctc_match_overflow;
 
  // Enable the compare match interrupt
  TIMSK2 |= (1 << OCIE2A);
 
  //REMEMBER TO ENABLE GLOBAL INTERRUPTS AFTER THIS WITH sei(); !!!
}

unsigned long millis ()
{
  unsigned long millis_return;
 
  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    millis_return = timer2_millis;
  }
  return millis_return;
} 

#endif /* MILLIS_H_ */
