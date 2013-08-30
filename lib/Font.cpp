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

#define C(c,r) ((c << 4) | r)

PROGMEM const uint8_t letters_33[] = { C(1,1), C(1,2), C(1,3), C(1,4), C(1,5), C(1,7), 255 };
PROGMEM const uint8_t letters_39[] = { C(1,3), C(2,1), C(2,2), 255 };
PROGMEM const uint8_t letters_44[] = { C(1,7), C(2,5), C(2,6), 255 };
PROGMEM const uint8_t letters_48[] = { C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(1,1), C(1,7), C(2,1), C(2,7), C(3,1), C(3,7), C(4,2), C(4,3), C(4,4), C(4,5), C(4,6), 255 };
PROGMEM const uint8_t letters_49[] = { C(1,2), C(1,7), C(2,1), C(2,2), C(2,3), C(2,4), C(2,5), C(2,6), C(2,7), C(3,7), 255 };
PROGMEM const uint8_t letters_50[] = { C(0,2), C(0,5), C(0,6), C(0,7), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,4), C(3,7), C(4,2), C(4,3), C(4,7), 255 };
PROGMEM const uint8_t letters_51[] = { C(0,2), C(0,6), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,4), C(3,7), C(4,2), C(4,3), C(4,5), C(4,6), 255 };
PROGMEM const uint8_t letters_52[] = { C(0,4), C(0,5), C(1,3), C(1,5), C(2,2), C(2,5), C(3,1), C(3,2), C(3,3), C(3,4), C(3,5), C(3,6), C(3,7), C(4,5), 255 };
PROGMEM const uint8_t letters_53[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,7), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,4), C(3,7), C(4,1), C(4,5), C(4,6), 255 };
PROGMEM const uint8_t letters_54[] = { C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,4), C(3,7), C(4,2), C(4,5), C(4,6), 255 };
PROGMEM const uint8_t letters_55[] = { C(0,1), C(1,1), C(2,1), C(2,5), C(2,6), C(2,7), C(3,1), C(3,3), C(3,4), C(4,1), C(4,2), 255 };
PROGMEM const uint8_t letters_56[] = { C(0,2), C(0,3), C(0,5), C(0,6), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,4), C(3,7), C(4,2), C(4,3), C(4,5), C(4,6), 255 };
PROGMEM const uint8_t letters_57[] = { C(0,2), C(0,3), C(0,7), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,4), C(3,7), C(4,2), C(4,3), C(4,4), C(4,5), C(4,6), 255 };
PROGMEM const uint8_t letters_65[] = { C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,1), C(1,4), C(2,1), C(2,4), C(3,2), C(3,3), C(3,4), C(3,5), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_66[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,2), C(3,3), C(3,5), C(3,6), 255 };
PROGMEM const uint8_t letters_67[] = { C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(1,1), C(1,7), C(2,1), C(2,7), C(3,1), C(3,7), 255 };
PROGMEM const uint8_t letters_68[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,1), C(1,7), C(2,1), C(2,7), C(3,2), C(3,3), C(3,4), C(3,5), C(3,6), 255 };
PROGMEM const uint8_t letters_69[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,4), C(3,7), 255 };
PROGMEM const uint8_t letters_70[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,1), C(1,4), C(2,1), C(2,4), C(3,1), C(3,4), 255 };
PROGMEM const uint8_t letters_71[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,1), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,2), C(3,4), C(3,5), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_72[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,4), C(2,4), C(3,1), C(3,2), C(3,3), C(3,4), C(3,5), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_73[] = { C(0,1), C(0,7), C(1,1), C(1,2), C(1,3), C(1,4), C(1,5), C(1,6), C(1,7), C(2,1), C(2,7), 255 };
PROGMEM const uint8_t letters_74[] = { C(0,7), C(1,1), C(1,7), C(2,1), C(2,2), C(2,3), C(2,4), C(2,5), C(2,6), C(3,1), 255 };
PROGMEM const uint8_t letters_75[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,4), C(2,3), C(2,5), C(3,1), C(3,2), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_76[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,7), C(2,7), C(3,7), 255 };
PROGMEM const uint8_t letters_77[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,2), C(2,3), C(3,2), C(4,1), C(4,2), C(4,3), C(4,4), C(4,5), C(4,6), C(4,7), 255 };
PROGMEM const uint8_t letters_78[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,2), C(2,3), C(3,4), C(4,1), C(4,2), C(4,3), C(4,4), C(4,5), C(4,6), C(4,7), 255 };
PROGMEM const uint8_t letters_79[] = { C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(1,1), C(1,7), C(2,1), C(2,7), C(3,2), C(3,3), C(3,4), C(3,5), C(3,6), 255 };
PROGMEM const uint8_t letters_80[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,1), C(1,4), C(2,1), C(2,4), C(3,2), C(3,3), 255 };
PROGMEM const uint8_t letters_81[] = { C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(1,1), C(1,7), C(2,1), C(2,5), C(2,7), C(3,1), C(3,6), C(4,2), C(4,3), C(4,4), C(4,5), C(4,7), 255 };
PROGMEM const uint8_t letters_82[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,1), C(1,4), C(2,1), C(2,4), C(2,5), C(3,1), C(3,4), C(3,6), C(4,2), C(4,3), C(4,7), 255 };
PROGMEM const uint8_t letters_83[] = { C(0,2), C(0,3), C(0,7), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,5), C(3,6), 255 };
PROGMEM const uint8_t letters_84[] = { C(0,1), C(1,1), C(1,2), C(1,3), C(1,4), C(1,5), C(1,6), C(1,7), C(2,1), 255 };
PROGMEM const uint8_t letters_85[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(1,7), C(2,7), C(3,1), C(3,2), C(3,3), C(3,4), C(3,5), C(3,6), 255 };
PROGMEM const uint8_t letters_86[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(1,6), C(2,7), C(3,6), C(4,1), C(4,2), C(4,3), C(4,4), C(4,5), 255 };
PROGMEM const uint8_t letters_87[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,6), C(2,5), C(3,6), C(4,1), C(4,2), C(4,3), C(4,4), C(4,5), C(4,6), C(4,7), 255 };
PROGMEM const uint8_t letters_88[] = { C(0,1), C(0,2), C(0,6), C(0,7), C(1,3), C(1,5), C(2,4), C(3,3), C(3,5), C(4,1), C(4,2), C(4,6), C(4,7), 255 };
PROGMEM const uint8_t letters_89[] = { C(0,1), C(0,2), C(1,3), C(2,4), C(2,5), C(2,6), C(2,7), C(3,3), C(4,1), C(4,2), 255 };
PROGMEM const uint8_t letters_90[] = { C(0,1), C(0,6), C(0,7), C(1,1), C(1,5), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,3), C(3,7), C(4,1), C(4,2), C(4,7), 255 };

#ifdef LOWERCASE
// This section of the font works now, but doesn't appear to be correct.
PROGMEM const uint8_t letters_97[] = { C(0,6), C(0,7), C(1,3), C(1,5), C(1,7), C(2,3), C(2,5), C(2,7), C(3,4), C(3,5), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_98[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,4), C(1,7), C(2,4), C(2,7), C(3,5), C(3,6), 255 };
PROGMEM const uint8_t letters_99[] = { C(0,5), C(0,6), C(1,4), C(1,7), C(2,4), C(2,7), 255 };
PROGMEM const uint8_t letters_100[] = { C(0,5), C(0,6), C(1,4), C(1,7), C(2,4), C(2,7), C(3,1), C(3,2), C(3,3), C(3,4), C(3,5), C(3,6), C(3,7), 255 };
//PROGMEM const uint8_t letters_100[] = { C(0,2), C(0,3), C(1,1), C(1,4), C(1,7), C(2,1), C(2,4), C(2,7), C(3,1), C(3,2), C(3,3), C(3,4), C(3,5), C(3,6), 255 };
PROGMEM const uint8_t letters_101[] = { C(0,4), C(0,5), C(0,6), C(1,3), C(1,5), C(1,7), C(2,3), C(2,5), C(2,7), C(3,4), C(3,5), C(3,7), 255 };
PROGMEM const uint8_t letters_102[] = { C(0,4), C(1,1), C(1,2), C(1,3), C(1,4), C(1,5), C(1,6), C(1,7), C(2,1), C(2,4), C(3,1), 255 };
PROGMEM const uint8_t letters_103[] = { C(0,4), C(0,5), C(0,8), C(1,3), C(1,6), C(1,8), C(2,3), C(2,6), C(2,8), C(3,3), C(3,4), C(3,5), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_104[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,4), C(2,4), C(3,5), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_105[] = { C(0,2), C(0,4), C(0,5), C(0,6), C(0,7), 255 };
PROGMEM const uint8_t letters_106[] = { C(0,8), C(1,2), C(1,4), C(1,5), C(1,6), C(1,7), C(1,8), 255 };
PROGMEM const uint8_t letters_107[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), C(1,5), C(2,4), C(2,6), C(3,3), C(3,7), 255 };
PROGMEM const uint8_t letters_108[] = { C(0,1), C(0,2), C(0,3), C(0,4), C(0,5), C(0,6), C(0,7), 255 };
PROGMEM const uint8_t letters_109[] = { C(0,4), C(0,5), C(0,6), C(0,7), C(1,4), C(2,5), C(2,6), C(2,7), C(3,4), C(4,5), C(4,6), C(4,7), 255 };
PROGMEM const uint8_t letters_110[] = { C(0,4), C(0,5), C(0,6), C(0,7), C(1,4), C(2,4), C(3,5), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_111[] = { C(0,5), C(0,6), C(1,4), C(1,7), C(2,4), C(2,7), C(3,5), C(3,6), 255 };
PROGMEM const uint8_t letters_112[] = { C(0,4), C(0,5), C(0,6), C(0,7), C(0,8), C(1,4), C(1,7), C(2,4), C(2,7), C(3,5), C(3,6), 255 };
PROGMEM const uint8_t letters_113[] = { C(0,5), C(0,6), C(1,4), C(1,7), C(2,4), C(2,7), C(3,4), C(3,5), C(3,6), C(3,7), C(3,8), 255 };
PROGMEM const uint8_t letters_114[] = { C(0,4), C(0,5), C(0,6), C(0,7), C(1,4), C(2,4), 255 };
PROGMEM const uint8_t letters_115[] = { C(0,3), C(0,4), C(0,7), C(1,3), C(1,5), C(1,7), C(2,3), C(2,5), C(2,7), C(3,3), C(3,5), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_116[] = { C(0,4), C(1,3), C(1,4), C(1,5), C(1,6), C(1,7), C(2,4), C(2,7), C(3,7), 255 };
PROGMEM const uint8_t letters_117[] = { C(0,4), C(0,5), C(0,6), C(1,7), C(2,7), C(3,4), C(3,5), C(3,6), C(3,7), 255 };
PROGMEM const uint8_t letters_118[] = { C(0,4), C(0,5), C(0,6), C(1,7), C(2,4), C(2,5), C(2,6), 255 };
PROGMEM const uint8_t letters_119[] = { C(0,4), C(0,5), C(0,6), C(1,7), C(2,4), C(2,5), C(2,6), C(3,7), C(4,4), C(4,5), C(4,6), 255 };
PROGMEM const uint8_t letters_120[] = { C(0,4), C(0,7), C(1,5), C(1,6), C(2,4), C(2,7), 255 };
#endif

const unsigned char fontMin=33;
#ifdef LOWERCASE
const unsigned char fontMax=120;
#else
const unsigned char fontMax=90;
#endif

PROGMEM const uint8_t* const font[] = {  letters_33 /*!*/,  0,  0,  0,  0,  0,  letters_39 /*'*/,  0,  0,  0,  0,  letters_44 /*,*/,  0,  0,  0,  
letters_48 /*0*/,  letters_49 /*1*/,  letters_50 /*2*/,  letters_51 /*3*/,  letters_52 /*4*/,  letters_53 /*5*/,  
letters_54 /*6*/,  letters_55 /*7*/,  letters_56 /*8*/,  letters_57 /*9*/,  0,  0,  0,  0,  0,  0,  0,  letters_65 /*A*/,  
letters_66 /*B*/,  letters_67 /*C*/,  letters_68 /*D*/,  letters_69 /*E*/,  letters_70 /*F*/,  letters_71 /*G*/,  
letters_72 /*H*/,  letters_73 /*I*/,  letters_74 /*J*/,  letters_75 /*K*/,  letters_76 /*L*/,  letters_77 /*M*/,  
letters_78 /*N*/,  letters_79 /*O*/,  letters_80 /*P*/,  letters_81 /*Q*/,  letters_82 /*R*/,  letters_83 /*S*/,  
letters_84 /*T*/,  letters_85 /*U*/,  letters_86 /*V*/,  letters_87 /*W*/,  letters_88 /*X*/,  letters_89 /*Y*/,  
letters_90 /*Z*/,
#ifdef LOWERCASE
0,  0,  0,  0,  0,  0,  letters_97 /*a*/,  letters_98 /*b*/,  letters_99 /*c*/,  letters_100 /*d*/,  letters_101 /*e*/,  letters_102 /*f*/,  letters_103 /*g*/,  letters_104 /*h*/,  letters_105 /*i*/,  letters_106 /*j*/,  letters_107 /*k*/,  letters_108 /*l*/,  letters_109 /*m*/,  letters_110 /*n*/,  letters_111 /*o*/,  letters_112 /*p*/,  letters_113 /*q*/,  letters_114 /*r*/,  letters_115 /*s*/,  letters_116 /*t*/,  letters_117 /*u*/,  letters_118 /*v*/,  letters_119 /*w*/,  letters_120 /*x*/,
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

  while (data != 255) {
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

  while (data != 255) {
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
