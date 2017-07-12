/*
 * Project: Lightweight millisecond tracking library
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/millisecond-tracking-library-for-avr/
 */

/* 
The millis() function known from Arduino
Calling millis() will return the milliseconds since the program started
Tested on atmega328p
Using content from http://www.adnbr.co.uk/articles/counting-milliseconds
Author: Monoclecat, https://github.com/monoclecat/avr-millis-function
REMEMBER: Add sei(); after init_millis() to enable global interrupts!
 */

#include "Millis.h"

volatile unsigned long timer3_millis = 0;
//NOTE: A unsigned long holds values from 0 to 4,294,967,295 (2^32 - 1). It will roll over to 0 after reaching its maximum value.

ISR(TIMER3_COMPA_vect)
{
  timer3_millis++;  
}

void init_millis(unsigned long f_cpu)
{
  unsigned long ctc_match_overflow;
  
  ctc_match_overflow = ((f_cpu / 1000) / 8); //when timer1 is this value, 1ms has passed
    
  // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
  TCCR3B |= (1 << WGM32) | (1 << CS31);
  
  // high byte first, then low byte
  OCR3AH = (ctc_match_overflow >> 8);
  OCR3AL = ctc_match_overflow;
 
  // Enable the compare match interrupt
  TIMSK3 |= (1 << OCIE3A);
 
  //REMEMBER TO ENABLE GLOBAL INTERRUPTS AFTER THIS WITH sei(); !!!
}

unsigned long millis()
{
  unsigned long millis_return;
 
  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    millis_return = timer3_millis;
  }
  return millis_return;
} 
