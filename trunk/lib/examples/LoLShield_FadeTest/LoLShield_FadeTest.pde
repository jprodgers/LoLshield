/*
 LoL Shield grayscale / fading test

 Writen for the LoL Shield, designed by Jimmie Rodgers:
 http://jimmieprodgers.com/kits/lolshield/

 Written by Thilo Fromm <kontakt@thilo-fromm.de> 
 largely based on the work of Matt Mets <Matt.Mets@gmail.com>.

 This is free software; you can redistribute it and/or
 modify it under the terms of the GNU Version 3 General Public
 License as published by the Free Software Foundation; 
 or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Charliplexing.h"

// Screen "refresh"
const unsigned int fps = 40;
const unsigned int fps_ms = 1000 / fps;

unsigned int frames = 0;

// number of frames to wait before advance animation
const unsigned int frames_per_step = 10;

void setup()                    // run once, when the sketch starts
{
  LedSign::Init(DOUBLE_BUFFER | GRAYSCALE);
}

uint8_t i = 0; 
void loop()                     // run over and over again
{ 
  static unsigned long start = 0;
  unsigned long end;

  for (int row = 0; row < DISPLAY_ROWS; row++)
    for (int col = 0; col < DISPLAY_COLS; col++)
      LedSign::Set(col, row, (row+col + i)%SHADES);

  LedSign::Flip(true);

  if (0 == (frames++) % frames_per_step)
    i++;

  end = millis();
  if (start) {
    unsigned long diff = end - start;
    if ( diff < fps_ms ) {
        delay( fps_ms - diff );
    } else {
        // Signal that we're too slow... 
        LedSign::Clear(0);
        LedSign::Flip();
        delay(500);
        LedSign::Clear(SHADES-1);
        LedSign::Flip();
        delay(500);
        // re-set timeout
        end = millis();
    }
  }
  start = end;
}
