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

const uint8_t PROGMEM letters_71[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,7, 2,1, 2,4, 2,7, 3,1, 3,2, 3,4, 3,5, 3,6, 3,7, 9,9 };
const uint8_t PROGMEM letters_83[] = { 0,2, 0,3, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,5, 3,6, 9,9 };
const uint8_t PROGMEM letters_67[] = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,7, 2,1, 2,7, 3,1, 3,7, 9,9 };
const uint8_t PROGMEM letters_76[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,7, 2,7, 3,7, 9,9 };
const uint8_t PROGMEM letters_89[] = { 0,1, 0,2, 1,3, 2,4, 2,5, 2,6, 2,7, 3,3, 4,1, 4,2, 9,9 };
const uint8_t PROGMEM letters_82[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 2,1, 2,4, 2,5, 3,1, 3,4, 3,6, 4,2, 4,3, 4,7, 9,9 };
const uint8_t PROGMEM letters_79[] = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,7, 2,1, 2,7, 3,2, 3,3, 3,4, 3,5, 3,6, 9,9 };
const uint8_t PROGMEM letters_33[] = { 1,1, 1,2, 1,3, 1,4, 1,5, 1,7, 9,9 };
const uint8_t PROGMEM letters_65[] = { 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 2,1, 2,4, 3,2, 3,3, 3,4, 3,5, 3,6, 3,7, 9,9 };
const uint8_t PROGMEM letters_87[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,6, 2,5, 3,6, 4,1, 4,2, 4,3, 4,4, 4,5, 4,6, 4,7, 9,9 };
const uint8_t PROGMEM letters_69[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 9,9 };
const uint8_t PROGMEM letters_80[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 2,1, 2,4, 3,2, 3,3, 9,9 };
const uint8_t PROGMEM letters_50[] = { 0,2, 0,5, 0,6, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,3, 4,7, 9,9 };
const uint8_t PROGMEM letters_49[] = { 1,2, 1,7, 2,1, 2,2, 2,3, 2,4, 2,5, 2,6, 2,7, 3,7, 9,9 };
const uint8_t PROGMEM letters_74[] = { 0,7, 1,1, 1,7, 2,1, 2,2, 2,3, 2,4, 2,5, 2,6, 3,1, 9,9 };
const uint8_t PROGMEM letters_68[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,7, 2,1, 2,7, 3,2, 3,3, 3,4, 3,5, 3,6, 9,9 };
const uint8_t PROGMEM letters_90[] = { 0,1, 0,6, 0,7, 1,1, 1,5, 1,7, 2,1, 2,4, 2,7, 3,1, 3,3, 3,7, 4,1, 4,2, 4,7, 9,9 };
const uint8_t PROGMEM letters_70[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 2,1, 2,4, 3,1, 3,4, 9,9 };
const uint8_t PROGMEM letters_88[] = { 0,1, 0,2, 0,6, 0,7, 1,3, 1,5, 2,4, 3,3, 3,5, 4,1, 4,2, 4,6, 4,7, 9,9 };
const uint8_t PROGMEM letters_75[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,4, 2,3, 2,5, 3,1, 3,2, 3,6, 3,7, 9,9 };
const uint8_t PROGMEM letters_53[] = { 0,1, 0,2, 0,3, 0,4, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,1, 4,5, 4,6, 9,9 };
const uint8_t PROGMEM letters_48[] = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,7, 2,1, 2,7, 3,1, 3,7, 4,2, 4,3, 4,4, 4,5, 4,6, 9,9 };
const uint8_t PROGMEM letters_77[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,2, 2,3, 3,2, 4,1, 4,2, 4,3, 4,4, 4,5, 4,6, 4,7, 9,9 };
const uint8_t PROGMEM letters_54[] = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,5, 4,6, 9,9 };
const uint8_t PROGMEM letters_39[] = { 1,3, 2,1, 2,2, 9,9 };
const uint8_t PROGMEM letters_85[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 1,7, 2,7, 3,1, 3,2, 3,3, 3,4, 3,5, 3,6, 9,9 };
const uint8_t PROGMEM letters_57[] = { 0,2, 0,3, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,3, 4,4, 4,5, 4,6, 9,9 };
const uint8_t PROGMEM letters_78[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,2, 2,3, 3,4, 4,1, 4,2, 4,3, 4,4, 4,5, 4,6, 4,7, 9,9 };
const uint8_t PROGMEM letters_84[] = { 0,1, 1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 2,1, 9,9 };
const uint8_t PROGMEM letters_81[] = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,7, 2,1, 2,5, 2,7, 3,1, 3,6, 4,2, 4,3, 4,4, 4,5, 4,7, 9,9 };
const uint8_t PROGMEM letters_51[] = { 0,2, 0,6, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,3, 4,5, 4,6, 9,9 };
const uint8_t PROGMEM letters_86[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 1,6, 2,7, 3,6, 4,1, 4,2, 4,3, 4,4, 4,5, 9,9 };
const uint8_t PROGMEM letters_72[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,4, 2,4, 3,1, 3,2, 3,3, 3,4, 3,5, 3,6, 3,7, 9,9 };
const uint8_t PROGMEM letters_73[] = { 0,1, 0,7, 1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 2,1, 2,7, 9,9 };
const uint8_t PROGMEM letters_44[] = { 1,7, 2,5, 2,6, 9,9 };
const uint8_t PROGMEM letters_56[] = { 0,2, 0,3, 0,5, 0,6, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,3, 4,5, 4,6, 9,9 };
const uint8_t PROGMEM letters_66[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,2, 3,3, 3,5, 3,6, 9,9 };
const uint8_t PROGMEM letters_52[] = { 0,4, 0,5, 1,3, 1,5, 2,2, 2,5, 3,1, 3,2, 3,3, 3,4, 3,5, 3,6, 3,7, 4,5, 9,9 };
const uint8_t PROGMEM letters_55[] = { 0,1, 1,1, 2,1, 2,5, 2,6, 2,7, 3,1, 3,3, 3,4, 4,1, 4,2, 9,9 };

const uint8_t* const font[] = {  letters_33 /*!*/,  0,  0,  0,  0,  0,  letters_39 /*'*/,  0,  0,  0,  0,  letters_44 /*,*/,  0,  0,  0,  
letters_48 /*0*/,  letters_49 /*1*/,  letters_50 /*2*/,  letters_51 /*3*/,  letters_52 /*4*/,  letters_53 /*5*/,  
letters_54 /*6*/,  letters_55 /*7*/,  letters_56 /*8*/,  letters_57 /*9*/,  0,  0,  0,  0,  0,  0,  0,  letters_65 /*A*/,  
letters_66 /*B*/,  letters_67 /*C*/,  letters_68 /*D*/,  letters_69 /*E*/,  letters_70 /*F*/,  letters_71 /*G*/,  
letters_72 /*H*/,  letters_73 /*I*/,  letters_74 /*J*/,  letters_75 /*K*/,  letters_76 /*L*/,  letters_77 /*M*/,  
letters_78 /*N*/,  letters_79 /*O*/,  letters_80 /*P*/,  letters_81 /*Q*/,  letters_82 /*R*/,  letters_83 /*S*/,  
letters_84 /*T*/,  letters_85 /*U*/,  letters_86 /*V*/,  letters_87 /*W*/,  letters_88 /*X*/,  letters_89 /*Y*/,  
letters_90 /*Z*/
};


const unsigned char fontMin=33;
const unsigned char fontMax=90;

#if 0

// This section of the font works now, but doesn't appear to be correct.

const uint8_t PROGMEM letters_117[] = { 0,4, 0,5, 0,6, 1,7, 2,7, 3,4, 3,5, 3,6, 3,7, 9,9 };
const uint8_t PROGMEM letters_104[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,4, 2,4, 3,5, 3,6, 3,7, 9,9 };
const uint8_t PROGMEM letters_109[] = { 0,4, 0,5, 0,6, 0,7, 1,4, 2,5, 2,6, 2,7, 3,4, 4,5, 4,6, 4,7, 9,9 };
const uint8_t PROGMEM letters_114[] = { 0,4, 0,5, 0,6, 0,7, 1,4, 2,4, 9,9 };
const uint8_t PROGMEM letters_108[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 9,9 };
const uint8_t PROGMEM letters_116[] = { 0,2, 1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 2,2, 2,7, 3,7, 9,9 };
const uint8_t PROGMEM letters_107[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,5, 2,4, 2,6, 3,3, 3,7, 9,9 };
const uint8_t PROGMEM letters_118[] = { 0,4, 0,5, 1,6, 2,7, 3,6, 4,4, 4,5, 9,9 };
const uint8_t PROGMEM letters_98[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,4, 1,7, 2,4, 2,7, 3,5, 3,6, 9,9 };
const uint8_t PROGMEM letters_120[] = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,5, 2,4, 2,6, 3,3, 3,7, 9,9 };
const uint8_t PROGMEM letters_119[] = { 0,4, 0,5, 1,6, 1,7, 2,4, 2,5, 3,6, 3,7, 4,4, 4,5, 9,9 };
const uint8_t PROGMEM letters_111[] = { 0,5, 0,6, 1,4, 1,7, 2,4, 2,7, 3,5, 3,6, 9,9 };
const uint8_t PROGMEM letters_105[] = { 0,1, 0,3, 0,4, 0,5, 0,6, 0,7, 9,9 };
const uint8_t PROGMEM letters_110[] = { 0,4, 0,5, 0,6, 0,7, 1,4, 2,4, 3,5, 3,6, 3,7, 9,9 };
const uint8_t PROGMEM letters_113[] = { 0,4, 0,5, 1,3, 1,6, 2,3, 2,6, 3,3, 3,4, 3,5, 3,6, 3,7, 3,8, 9,9 };
const uint8_t PROGMEM letters_100[] = { 0,2, 0,3, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,2, 3,3, 3,4, 3,5, 3,6, 9,9 };
const uint8_t PROGMEM letters_103[] = { 0,3, 0,4, 0,8, 1,2, 1,5, 1,8, 2,2, 2,5, 2,8, 3,2, 3,3, 3,4, 3,5, 3,6, 3,7, 9,9 };
const uint8_t PROGMEM letters_112[] = { 0,3, 0,4, 0,5, 0,6, 0,7, 0,8, 1,3, 1,6, 2,3, 2,6, 3,4, 3,5, 9,9 };
const uint8_t PROGMEM letters_101[] = { 0,4, 0,5, 0,6, 1,3, 1,5, 1,7, 2,3, 2,5, 2,7, 3,4, 3,5, 3,7, 9,9 };
const uint8_t PROGMEM letters_102[] = { 0,5, 1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 2,1, 2,5, 3,1, 9,9 };
const uint8_t PROGMEM letters_115[] = { 0,3, 0,4, 0,7, 1,3, 1,5, 1,7, 2,3, 2,5, 2,7, 3,3, 3,5, 3,6, 3,7, 9,9 };
const uint8_t PROGMEM letters_99[] = { 0,5, 0,6, 1,4, 1,7, 2,4, 2,7, 3,4, 3,7, 9,9 };
const uint8_t PROGMEM letters_106[] = { 0,8, 1,2, 1,4, 1,5, 1,6, 1,7, 1,8, 9,9 };
const uint8_t PROGMEM letters_97[] = { 0,6, 0,7, 1,3, 1,5, 1,7, 2,3, 2,5, 2,7, 3,4, 3,5, 3,6, 3,7, 9,9 };

const uint8_t* const font2[] = { 0,  0,  0,  0,  0,  0,  letters_97 /*a*/,  letters_98 /*b*/,  letters_99 /*c*/,  letters_100 /*d*/,  letters_101 /*e*/,  letters_102 /*f*/,  letters_103 /*g*/,  letters_104 /*h*/,  letters_105 /*i*/,  letters_106 /*j*/,  letters_107 /*k*/,  letters_108 /*l*/,  letters_109 /*m*/,  letters_110 /*n*/,  letters_111 /*o*/,  letters_112 /*p*/,  letters_113 /*q*/,  letters_114 /*r*/,  letters_115 /*s*/,  letters_116 /*t*/,  letters_117 /*u*/,  letters_118 /*v*/,  letters_119 /*w*/,  letters_120 /*x*/ };
*/

#endif

/* -----------------------------------------------------------------  */
/** Draws a figure (0-9). You should call it with c=1, 
 * wait a little them call it again with c=0
 * @param figure is the figure [0-9]
 * @param x,y coordinates, 
 * @param c is 1 or 0 to draw or clear it
 */
uint8_t Font::Draw(unsigned char letter, int x, int y, uint8_t c) {
  uint8_t maxx=0;

  uint8_t charCol;
  uint8_t charRow;

  const uint8_t* character;
  if (letter==' ') return 3+2;
  if (letter<fontMin || letter>fontMax) {
    return 0;
  }

//  if (letter>90) {
//     character = font2[letter-90];
//  } else {
    character = font[letter-fontMin];
//  }

  charCol = pgm_read_byte_near(character++);
  charRow = pgm_read_byte_near(character++);

  while (charRow!=9) {
    if (charCol>maxx) maxx=charCol;
    if (
     charCol + x <DISPLAY_COLS && 
     charCol + x >=0 && 
     charRow + y <DISPLAY_ROWS && 
     charRow + y >=0
    ) {
        LedSign::Set(charCol + x, charRow+y, c);
    } 

    charCol = pgm_read_byte_near(character++);
    charRow = pgm_read_byte_near(character++);
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
  uint8_t maxx=0;

  uint8_t charCol;
  uint8_t charRow;

  const uint8_t* character;
  if (letter==' ') return 3+2;
  if (letter<fontMin || letter>fontMax) {
    return 0;
  }

//  if (letter>90) {
//     character = font2[letter-90];
//  } else {
    character = font[letter-fontMin];
//  }

  charCol = pgm_read_byte_near(character++);
  charRow = pgm_read_byte_near(character++);

  while (charRow!=9) {
    if (charCol>maxx) maxx=charCol;
    if (
     charRow + x <DISPLAY_COLS && 
     charRow + x >=0 && 
     charCol + y <DISPLAY_ROWS && 
     charCol + y >=0
    ) {
        LedSign::Set(7 - charRow + x, charCol + y, c);
    } 

    charCol = pgm_read_byte_near(character++);
    charRow = pgm_read_byte_near(character++);
  }
  return maxx+2;
}





