/*
  TEXT SAMPLE CODE for LOL Shield for Arduino
  Copyright 2009/2010 Benjamin Sonntag <benjamin@sonntag.fr> http://benjamin.sonntag.fr/
  
  History:
  	2009-12-31 - V1.0 FONT Drawing, at Berlin after 26C3 ;) 

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#include "Charliplexing.h"
#include "Font.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif





/* -----------------------------------------------------------------  */
/** MAIN program Setup
 */
void setup()                    // run once, when the sketch starts
{
  LedSign::Init();
}


/* -----------------------------------------------------------------  */
/** MAIN program Loop
 */
static const char test[]="HELLO WORLD!   ";

void loop()                     // run over and over again
{

    for (int8_t x=DISPLAY_COLS, i=0;; x--) {
	LedSign::Clear();
        for (int8_t x2=x, i2=i; x2<DISPLAY_COLS;) {
	    int8_t w = Font::Draw(test[i2], x2, 0);
	    x2 += w, i2 = (i2+1)%strlen(test);
	    if (x2 <= 0)	// off the display completely?
		x = x2, i = i2;
	}
        delay(80);
    }
}
