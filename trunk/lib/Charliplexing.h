/*
  Charliplexing.h - Library for controlling the charliplexed led board
  from JimmiePRodgers.com
  Created by Alex Wenger, December 30, 2009.
  Released into the public domain.
*/

#ifndef Charliplexing_h
#define Charliplexing_h

#include <inttypes.h>

namespace LedSign
{
    extern void Init(void);
    extern void Set(uint8_t x, uint8_t y, uint8_t c);
    extern volatile unsigned int tcnt2;
    extern void Clear(int set=0);
    extern void Horizontal(int y, int set=0);
    extern void Vertical(int x, int set=0);
};

#endif
