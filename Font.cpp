#include <avr/pgmspace.h>

/*
  Font drawing library

  Copyright 2009/2010 Benjamin Sonntag <benjamin@sonntag.fr> http://benjamin.sonntag.fr/
  
  History:
  	2010-01-01 - V0.0 Initial code at Berlin after 26C3

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#include "Font.h"
#include "Charliplexing.h"
#include <inttypes.h>

#define L(x) letter_##x
#define C(x) PROGMEM const uint8_t L(x)[]
#define P(c,r) ((c << 4) | r)
#define	END 255

C(33) = { P(1,1), P(1,2), P(1,3), P(1,4), P(1,5), P(1,7), END };
C(39) = { P(1,3), P(2,1), P(2,2), END };
C(44) = { P(1,7), P(2,5), P(2,6), END };
C(48) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,7), P(3,1), P(3,7), P(4,2), P(4,3), P(4,4), P(4,5), P(4,6), END };
C(49) = { P(1,2), P(1,7), P(2,1), P(2,2), P(2,3), P(2,4), P(2,5), P(2,6), P(2,7), P(3,7), END };
C(50) = { P(0,2), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,3), P(4,7), END };
C(51) = { P(0,2), P(0,6), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,3), P(4,5), P(4,6), END };
C(52) = { P(0,4), P(0,5), P(1,3), P(1,5), P(2,2), P(2,5), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), P(4,5), END };
C(53) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,1), P(4,5), P(4,6), END };
C(54) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,5), P(4,6), END };
C(55) = { P(0,1), P(1,1), P(2,1), P(2,5), P(2,6), P(2,7), P(3,1), P(3,3), P(3,4), P(4,1), P(4,2), END };
C(56) = { P(0,2), P(0,3), P(0,5), P(0,6), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,3), P(4,5), P(4,6), END };
C(57) = { P(0,2), P(0,3), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,3), P(4,4), P(4,5), P(4,6), END };
C(65) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(2,1), P(2,4), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(66) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,2), P(3,3), P(3,5), P(3,6), END };
C(67) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,7), P(3,2), P(3,6), END };
C(68) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,7), P(2,1), P(2,7), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), END };
C(69) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,7), END };
C(70) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(2,1), P(2,4), P(3,1), END };
C(71) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,5), P(2,7), P(3,2), P(3,5), P(3,6), P(3,7), END };
C(72) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,4), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(73) = { P(0,1), P(0,7), P(1,1), P(1,2), P(1,3), P(1,4), P(1,5), P(1,6), P(1,7), P(2,1), P(2,7), END };
C(74) = { P(0,7), P(1,1), P(1,7), P(2,1), P(2,2), P(2,3), P(2,4), P(2,5), P(2,6), P(3,1), END };
C(75) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,3), P(2,5), P(3,1), P(3,2), P(3,6), P(3,7), END };
C(76) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,7), P(2,7), P(3,7), END };
C(77) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,2), P(2,3), P(3,2), P(4,1), P(4,2), P(4,3), P(4,4), P(4,5), P(4,6), P(4,7), END };
C(78) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,3), P(2,4), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(79) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,7), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), END };
C(80) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(2,1), P(2,4), P(3,2), P(3,3), END };
C(81) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,6), P(3,2), P(3,3), P(3,4), P(3,5), P(3,7), END };
C(82) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(2,1), P(2,4), P(2,5), P(3,2), P(3,3), P(3,6), P(3,7), END };
C(83) = { P(0,2), P(0,3), P(0,6), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,2), P(3,5), P(3,6), END };
C(84) = { P(0,1), P(1,1), P(1,2), P(1,3), P(1,4), P(1,5), P(1,6), P(1,7), P(2,1), END };
C(85) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,7), P(2,7), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), END };
C(86) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(1,6), P(2,7), P(3,6), P(4,1), P(4,2), P(4,3), P(4,4), P(4,5), END };
C(87) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,7), P(2,4), P(2,5), P(2,6), P(3,7), P(4,1), P(4,2), P(4,3), P(4,4), P(4,5), P(4,6), END };
C(88) = { P(0,1), P(0,2), P(0,6), P(0,7), P(1,3), P(1,5), P(2,4), P(3,3), P(3,5), P(4,1), P(4,2), P(4,6), P(4,7), END };
C(89) = { P(0,1), P(0,2), P(1,3), P(2,4), P(2,5), P(2,6), P(2,7), P(3,3), P(4,1), P(4,2), END };
C(90) = { P(0,1), P(0,6), P(0,7), P(1,1), P(1,5), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,3), P(3,7), P(4,1), P(4,2), P(4,7), END };

#ifdef LOWERCASE
C(97) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(98) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(1,7), P(2,4), P(2,7), P(3,5), P(3,6), END };
C(99) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), END };
C(100) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(101) = { P(0,4), P(0,5), P(0,6), P(1,3), P(1,5), P(1,7), P(2,3), P(2,5), P(2,7), P(3,4), P(3,5), P(3,7), END };
C(102) = { P(0,4), P(1,1), P(1,2), P(1,3), P(1,4), P(1,5), P(1,6), P(1,7), P(2,1), P(2,4), P(3,1), END };
C(103) = { P(0,4), P(0,5), P(0,8), P(1,3), P(1,6), P(1,8), P(2,3), P(2,6), P(2,8), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(104) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,4), P(3,5), P(3,6), P(3,7), END };
C(105) = { P(0,2), P(0,4), P(0,5), P(0,6), P(0,7), END };
C(106) = { P(0,8), P(1,2), P(1,4), P(1,5), P(1,6), P(1,7), P(1,8), END };
C(107) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,5), P(2,4), P(2,6), P(3,7), END };
C(108) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), END };
C(109) = { P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,5), P(2,6), P(2,7), P(3,4), P(4,5), P(4,6), P(4,7), END };
C(110) = { P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,4), P(3,5), P(3,6), P(3,7), END };
C(111) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), P(3,5), P(3,6), END };
C(112) = { P(0,4), P(0,5), P(0,6), P(0,7), P(0,8), P(1,4), P(1,7), P(2,4), P(2,7), P(3,5), P(3,6), END };
C(113) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), P(3,4), P(3,5), P(3,6), P(3,7), P(3,8), END };
C(114) = { P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,4), END };
C(115) = { P(0,4), P(0,7), P(1,3), P(1,5), P(1,7), P(2,3), P(2,5), P(2,7), P(3,3), P(3,6), END };
C(116) = { P(0,4), P(1,3), P(1,4), P(1,5), P(1,6), P(1,7), P(2,4), P(2,7), P(3,7), END };
C(117) = { P(0,4), P(0,5), P(0,6), P(1,7), P(2,7), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(118) = { P(0,4), P(0,5), P(0,6), P(1,7), P(2,4), P(2,5), P(2,6), END };
C(119) = { P(0,4), P(0,5), P(0,6), P(1,7), P(2,4), P(2,5), P(2,6), P(3,7), P(4,4), P(4,5), P(4,6), P(4,7), END };
C(120) = { P(0,4), P(0,7), P(1,5), P(1,6), P(2,4), P(2,7), END };
C(121) = { P(0,4), P(0,5), P(0,6), P(1,7), P(2,7), P(3,4), P(3,5), P(3,6), P(3,7), P(3,8), END };
C(122) = { P(0,4), P(0,7), P(1,4), P(1,6), P(1,7), P(2,4), P(2,5), P(2,7), P(3,4), P(3,7), END };
#endif

const unsigned char fontMin=33;
#ifdef LOWERCASE
const unsigned char fontMax=122;
#else
const unsigned char fontMax=90;
#endif

PROGMEM const uint8_t* const font[fontMax-fontMin+1] = {
  L(33)  /*!*/, 0, 0, 0, 0, 0,
  L(39)  /*'*/, 0, 0, 0, 0,
  L(44)  /*,*/, 0, 0, 0,  
  L(48)  /*0*/, L(49)  /*1*/, L(50)  /*2*/, L(51)  /*3*/, L(52)  /*4*/,
  L(53)  /*5*/, L(54)  /*6*/, L(55)  /*7*/, L(56)  /*8*/, L(57)  /*9*/,
  0, 0, 0, 0, 0, 0, 0,
  L(65)  /*A*/, L(66)  /*B*/, L(67)  /*C*/, L(68)  /*D*/, L(69)  /*E*/,
  L(70)  /*F*/, L(71)  /*G*/, L(72)  /*H*/, L(73)  /*I*/, L(74)  /*J*/,
  L(75)  /*K*/, L(76)  /*L*/, L(77)  /*M*/, L(78)  /*N*/, L(79)  /*O*/,
  L(80)  /*P*/, L(81)  /*Q*/, L(82)  /*R*/, L(83)  /*S*/, L(84)  /*T*/,
  L(85)  /*U*/, L(86)  /*V*/, L(87)  /*W*/, L(88)  /*X*/, L(89)  /*Y*/,
  L(90)  /*Z*/,
#ifdef LOWERCASE
  0, 0, 0, 0, 0, 0,
  L(97)  /*a*/, L(98)  /*b*/, L(99)  /*c*/, L(100) /*d*/, L(101) /*e*/,
  L(102) /*f*/, L(103) /*g*/, L(104) /*h*/, L(105) /*i*/, L(106) /*j*/,
  L(107) /*k*/, L(108) /*l*/, L(109) /*m*/, L(110) /*n*/, L(111) /*o*/,
  L(112) /*p*/, L(113) /*q*/, L(114) /*r*/, L(115) /*s*/, L(116) /*t*/,
  L(117) /*u*/, L(118) /*v*/, L(119) /*w*/, L(120) /*x*/, L(121) /*y*/,
  L(122) /*z*/,
#endif
};

/* -----------------------------------------------------------------  */
/** Draws a figure (0-9). You should call it with c=1, 
 * wait a little them call it again with c=0
 * @param figure is the figure [0-9]
 * @param x,y coordinates, 
 * @param c is 1 or 0 to draw or clear it
 */
uint8_t Font::Draw(unsigned char letter, int x, int y, uint8_t c) {
  uint8_t maxx=0, data;

  const uint8_t* character;
  if (letter==' ') return 3+2;
  if (letter<fontMin || letter>fontMax) {
    return 0;
  }

  character = (const uint8_t *)pgm_read_word_near(&font[letter-fontMin]);
  data = pgm_read_byte_near(character++);

  while (data != END) {
    uint8_t charCol = data >> 4, charRow = data & 15;
    if (charCol>maxx) maxx=charCol;
    if (
     charCol + x <DISPLAY_COLS && 
     charCol + x >=0 && 
     charRow + y <DISPLAY_ROWS && 
     charRow + y >=0
    ) {
        LedSign::Set(charCol + x, charRow+y, c);
    } 

    data = pgm_read_byte_near(character++);
  }
  return maxx+2;
}


/* -----------------------------------------------------------------  */
/** Draw a figure in the other direction (rotated 90°)
 * @param figure is the figure [0-9]
 * @param x,y coordinates, 
 * @param c is 1 or 0 to draw or clear it
*/
uint8_t Font::Draw90(unsigned char letter, int x, int y, uint8_t c) {
  uint8_t maxx=0, data;

  const uint8_t* character;
  if (letter==' ') return 3+2;
  if (letter<fontMin || letter>fontMax) {
    return 0;
  }

  character = (const uint8_t *)pgm_read_word_near(&font[letter-fontMin]);
  data = pgm_read_byte_near(character++);

  while (data != END) {
    uint8_t charCol = data >> 4, charRow = data & 15;
    if (charCol>maxx) maxx=charCol;
    if (
     7 - charRow + x <DISPLAY_COLS && 
     7 - charRow + x >=0 && 
     charCol + y <DISPLAY_ROWS && 
     charCol + y >=0
    ) {
        LedSign::Set(7 - charRow + x, charCol + y, c);
    } 

    data = pgm_read_byte_near(character++);
  }
  return maxx+2;
}
