#include "TrigFunctions.h"

// From https://github.com/FastLED/FastLED/blob/master/lib8tion/trig8.h

const uint8_t b_m16_interleave[] = { 0, 49, 49, 41, 90, 27, 117, 10 };

uint8_t sin8(uint8_t theta)
{
    uint8_t offset = theta;

    asm volatile(
                 "sbrc %[theta],6         \n\t"
                 "com  %[offset]           \n\t"
                 : [theta] "+r" (theta), [offset] "+r" (offset)
                 );

    offset &= 0x3F; // 0..63

    uint8_t secoffset  = offset & 0x0F; // 0..15
    if( theta & 0x40) secoffset++;

    uint8_t m16; uint8_t b;

    uint8_t section = offset >> 4; // 0..3
    uint8_t s2 = section * 2;

    const uint8_t* p = b_m16_interleave;
    p += s2;
    b   = *p;
    p++;
    m16 = *p;

    uint8_t mx;
    uint8_t xr1;
    asm volatile(
                 "mul %[m16],%[secoffset]   \n\t"
                 "mov %[mx],r0              \n\t"
                 "mov %[xr1],r1             \n\t"
                 "eor  r1, r1               \n\t"
                 "swap %[mx]                \n\t"
                 "andi %[mx],0x0F           \n\t"
                 "swap %[xr1]               \n\t"
                 "andi %[xr1], 0xF0         \n\t"
                 "or   %[mx], %[xr1]        \n\t"
                 : [mx] "=d" (mx), [xr1] "=d" (xr1)
                 : [m16] "d" (m16), [secoffset] "d" (secoffset)
                 );

    int8_t y = mx + b;
    if( theta & 0x80 ) y = -y;

    y += 128;

    return y;
}

uint8_t cos8(uint8_t theta)
{
    return sin8( theta + 64);
}
