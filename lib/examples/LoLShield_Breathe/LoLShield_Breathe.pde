/*
 Super-simple LoL Shield "breathe" fading test
 Written by Thilo Fromm <kontakt@thilo-fromm.de>.

 Writen for the LoL Shield, designed by Jimmie Rodgers:
 http://jimmieprodgers.com/kits/lolshield/

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

const unsigned int inhale_time_ms  = 500;
const unsigned int hold_breath_ms  = 600;
const unsigned int exhale_time_ms  = 800;
const unsigned int pause_breath_ms = 2000;

void setup()                    // run once, when the sketch starts
{
    LedSign::Init(DOUBLE_BUFFER | GRAYSCALE);
}

void loop()                     // run over and over again
{ 
    // inhale
    for (int8_t i=0; i <= SHADES-1; i++) {
        uint8_t sleep =   inhale_time_ms / SHADES 
                        + ( SHADES / 2 - i ) * ( inhale_time_ms / (SHADES * 6) );
        LedSign::Clear(i);
        LedSign::Flip(true);
        delay( sleep );
    }
    delay( hold_breath_ms );

    // exhale
    for (int8_t i=SHADES-1; i >= 0; i--) {
        uint8_t sleep =   exhale_time_ms / SHADES 
                        + ( SHADES / 2 - i ) * ( inhale_time_ms / (SHADES * 6) );
        LedSign::Clear(i);
        LedSign::Flip(true);
        delay( sleep );
    }

    // pause
    delay( pause_breath_ms );
}
