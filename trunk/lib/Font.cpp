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

prog_uchar letters_71[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,7, 2,1, 2,4, 2,7, 3,1, 3,2, 3,4, 3,5, 3,6, 3,7, 9,9 };
prog_uchar letters_83[] PROGMEM = { 0,2, 0,3, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,5, 3,6, 9,9 };
prog_uchar letters_67[] PROGMEM = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,7, 2,1, 2,7, 3,1, 3,7, 9,9 };
prog_uchar letters_76[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,7, 2,7, 3,7, 9,9 };
prog_uchar letters_89[] PROGMEM = { 0,1, 0,2, 1,3, 2,4, 2,5, 2,6, 2,7, 3,3, 4,1, 4,2, 9,9 };
prog_uchar letters_82[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 2,1, 2,4, 2,5, 3,1, 3,4, 3,6, 4,2, 4,3, 4,7, 9,9 };
prog_uchar letters_79[] PROGMEM = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,7, 2,1, 2,7, 3,2, 3,3, 3,4, 3,5, 3,6, 9,9 };
prog_uchar letters_33[] PROGMEM = { 1,1, 1,2, 1,3, 1,4, 1,5, 1,7, 9,9 };
prog_uchar letters_65[] PROGMEM = { 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 2,1, 2,4, 3,2, 3,3, 3,4, 3,5, 3,6, 3,7, 9,9 };
prog_uchar letters_87[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,6, 2,5, 3,6, 4,1, 4,2, 4,3, 4,4, 4,5, 4,6, 4,7, 9,9 };
prog_uchar letters_69[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 9,9 };
prog_uchar letters_80[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 2,1, 2,4, 3,2, 3,3, 9,9 };
prog_uchar letters_50[] PROGMEM = { 0,2, 0,5, 0,6, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,3, 4,7, 9,9 };
prog_uchar letters_49[] PROGMEM = { 1,2, 1,7, 2,1, 2,2, 2,3, 2,4, 2,5, 2,6, 2,7, 3,7, 9,9 };
prog_uchar letters_74[] PROGMEM = { 0,7, 1,1, 1,7, 2,1, 2,2, 2,3, 2,4, 2,5, 2,6, 3,1, 9,9 };
prog_uchar letters_68[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,7, 2,1, 2,7, 3,2, 3,3, 3,4, 3,5, 3,6, 9,9 };
prog_uchar letters_90[] PROGMEM = { 0,1, 0,6, 0,7, 1,1, 1,5, 1,7, 2,1, 2,4, 2,7, 3,1, 3,3, 3,7, 4,1, 4,2, 4,7, 9,9 };
prog_uchar letters_70[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 2,1, 2,4, 3,1, 3,4, 9,9 };
prog_uchar letters_88[] PROGMEM = { 0,1, 0,2, 0,6, 0,7, 1,3, 1,5, 2,4, 3,3, 3,5, 4,1, 4,2, 4,6, 4,7, 9,9 };
prog_uchar letters_75[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,4, 2,3, 2,5, 3,1, 3,2, 3,6, 3,7, 9,9 };
prog_uchar letters_53[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,1, 4,5, 4,6, 9,9 };
prog_uchar letters_48[] PROGMEM = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,7, 2,1, 2,7, 3,1, 3,7, 4,2, 4,3, 4,4, 4,5, 4,6, 9,9 };
prog_uchar letters_77[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,2, 2,3, 3,2, 4,1, 4,2, 4,3, 4,4, 4,5, 4,6, 4,7, 9,9 };
prog_uchar letters_54[] PROGMEM = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,5, 4,6, 9,9 };
prog_uchar letters_39[] PROGMEM = { 1,3, 2,1, 2,2, 9,9 };
prog_uchar letters_85[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 1,7, 2,7, 3,1, 3,2, 3,3, 3,4, 3,5, 3,6, 9,9 };
prog_uchar letters_57[] PROGMEM = { 0,2, 0,3, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,3, 4,4, 4,5, 4,6, 9,9 };
prog_uchar letters_78[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,2, 2,3, 3,4, 4,1, 4,2, 4,3, 4,4, 4,5, 4,6, 4,7, 9,9 };
prog_uchar letters_84[] PROGMEM = { 0,1, 1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 2,1, 9,9 };
prog_uchar letters_81[] PROGMEM = { 0,2, 0,3, 0,4, 0,5, 0,6, 1,1, 1,7, 2,1, 2,5, 2,7, 3,1, 3,6, 4,2, 4,3, 4,4, 4,5, 4,7, 9,9 };
prog_uchar letters_51[] PROGMEM = { 0,2, 0,6, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,3, 4,5, 4,6, 9,9 };
prog_uchar letters_86[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 1,6, 2,7, 3,6, 4,1, 4,2, 4,3, 4,4, 4,5, 9,9 };
prog_uchar letters_72[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,4, 2,4, 3,1, 3,2, 3,3, 3,4, 3,5, 3,6, 3,7, 9,9 };
prog_uchar letters_73[] PROGMEM = { 0,1, 0,7, 1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 2,1, 2,7, 9,9 };
prog_uchar letters_44[] PROGMEM = { 1,7, 2,5, 2,6, 9,9 };
prog_uchar letters_56[] PROGMEM = { 0,2, 0,3, 0,5, 0,6, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,4, 3,7, 4,2, 4,3, 4,5, 4,6, 9,9 };
prog_uchar letters_66[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,2, 3,3, 3,5, 3,6, 9,9 };
prog_uchar letters_52[] PROGMEM = { 0,4, 0,5, 1,3, 1,5, 2,2, 2,5, 3,1, 3,2, 3,3, 3,4, 3,5, 3,6, 3,7, 4,5, 9,9 };
prog_uchar letters_55[] PROGMEM = { 0,1, 1,1, 2,1, 2,5, 2,6, 2,7, 3,1, 3,3, 3,4, 4,1, 4,2, 9,9 };

prog_uchar* font[] = {  letters_33 /*!*/,  0,  0,  0,  0,  0,  letters_39 /*'*/,  0,  0,  0,  0,  letters_44 /*,*/,  0,  0,  0,  
letters_48 /*0*/,  letters_49 /*1*/,  letters_50 /*2*/,  letters_51 /*3*/,  letters_52 /*4*/,  letters_53 /*5*/,  
letters_54 /*6*/,  letters_55 /*7*/,  letters_56 /*8*/,  letters_57 /*9*/,  0,  0,  0,  0,  0,  0,  0,  letters_65 /*A*/,  
letters_66 /*B*/,  letters_67 /*C*/,  letters_68 /*D*/,  letters_69 /*E*/,  letters_70 /*F*/,  letters_71 /*G*/,  
letters_72 /*H*/,  letters_73 /*I*/,  letters_74 /*J*/,  letters_75 /*K*/,  letters_76 /*L*/,  letters_77 /*M*/,  
letters_78 /*N*/,  letters_79 /*O*/,  letters_80 /*P*/,  letters_81 /*Q*/,  letters_82 /*R*/,  letters_83 /*S*/,  
letters_84 /*T*/,  letters_85 /*U*/,  letters_86 /*V*/,  letters_87 /*W*/,  letters_88 /*X*/,  letters_89 /*Y*/,  
letters_90 /*Z*/
};


uint16_t fontMin=33;
uint16_t fontMax=90;

#if 0

// This section of the font works now, but doesn't appear to be correct.

prog_uchar letters_117[] PROGMEM = { 0,4, 0,5, 0,6, 1,7, 2,7, 3,4, 3,5, 3,6, 3,7, 9,9 };
prog_uchar letters_104[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,4, 2,4, 3,5, 3,6, 3,7, 9,9 };
prog_uchar letters_109[] PROGMEM = { 0,4, 0,5, 0,6, 0,7, 1,4, 2,5, 2,6, 2,7, 3,4, 4,5, 4,6, 4,7, 9,9 };
prog_uchar letters_114[] PROGMEM = { 0,4, 0,5, 0,6, 0,7, 1,4, 2,4, 9,9 };
prog_uchar letters_108[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 9,9 };
prog_uchar letters_116[] PROGMEM = { 0,2, 1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 2,2, 2,7, 3,7, 9,9 };
prog_uchar letters_107[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,5, 2,4, 2,6, 3,3, 3,7, 9,9 };
prog_uchar letters_118[] PROGMEM = { 0,4, 0,5, 1,6, 2,7, 3,6, 4,4, 4,5, 9,9 };
prog_uchar letters_98[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,4, 1,7, 2,4, 2,7, 3,5, 3,6, 9,9 };
prog_uchar letters_120[] PROGMEM = { 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7, 1,5, 2,4, 2,6, 3,3, 3,7, 9,9 };
prog_uchar letters_119[] PROGMEM = { 0,4, 0,5, 1,6, 1,7, 2,4, 2,5, 3,6, 3,7, 4,4, 4,5, 9,9 };
prog_uchar letters_111[] PROGMEM = { 0,5, 0,6, 1,4, 1,7, 2,4, 2,7, 3,5, 3,6, 9,9 };
prog_uchar letters_105[] PROGMEM = { 0,1, 0,3, 0,4, 0,5, 0,6, 0,7, 9,9 };
prog_uchar letters_110[] PROGMEM = { 0,4, 0,5, 0,6, 0,7, 1,4, 2,4, 3,5, 3,6, 3,7, 9,9 };
prog_uchar letters_113[] PROGMEM = { 0,4, 0,5, 1,3, 1,6, 2,3, 2,6, 3,3, 3,4, 3,5, 3,6, 3,7, 3,8, 9,9 };
prog_uchar letters_100[] PROGMEM = { 0,2, 0,3, 1,1, 1,4, 1,7, 2,1, 2,4, 2,7, 3,1, 3,2, 3,3, 3,4, 3,5, 3,6, 9,9 };
prog_uchar letters_103[] PROGMEM = { 0,3, 0,4, 0,8, 1,2, 1,5, 1,8, 2,2, 2,5, 2,8, 3,2, 3,3, 3,4, 3,5, 3,6, 3,7, 9,9 };
prog_uchar letters_112[] PROGMEM = { 0,3, 0,4, 0,5, 0,6, 0,7, 0,8, 1,3, 1,6, 2,3, 2,6, 3,4, 3,5, 9,9 };
prog_uchar letters_101[] PROGMEM = { 0,4, 0,5, 0,6, 1,3, 1,5, 1,7, 2,3, 2,5, 2,7, 3,4, 3,5, 3,7, 9,9 };
prog_uchar letters_102[] PROGMEM = { 0,5, 1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 2,1, 2,5, 3,1, 9,9 };
prog_uchar letters_115[] PROGMEM = { 0,3, 0,4, 0,7, 1,3, 1,5, 1,7, 2,3, 2,5, 2,7, 3,3, 3,5, 3,6, 3,7, 9,9 };
prog_uchar letters_99[] PROGMEM = { 0,5, 0,6, 1,4, 1,7, 2,4, 2,7, 3,4, 3,7, 9,9 };
prog_uchar letters_106[] PROGMEM = { 0,8, 1,2, 1,4, 1,5, 1,6, 1,7, 1,8, 9,9 };
prog_uchar letters_97[] PROGMEM = { 0,6, 0,7, 1,3, 1,5, 1,7, 2,3, 2,5, 2,7, 3,4, 3,5, 3,6, 3,7, 9,9 };

prog_uchar* font2[] = { 0,  0,  0,  0,  0,  0,  letters_97 /*a*/,  letters_98 /*b*/,  letters_99 /*c*/,  letters_100 /*d*/,  letters_101 /*e*/,  letters_102 /*f*/,  letters_103 /*g*/,  letters_104 /*h*/,  letters_105 /*i*/,  letters_106 /*j*/,  letters_107 /*k*/,  letters_108 /*l*/,  letters_109 /*m*/,  letters_110 /*n*/,  letters_111 /*o*/,  letters_112 /*p*/,  letters_113 /*q*/,  letters_114 /*r*/,  letters_115 /*s*/,  letters_116 /*t*/,  letters_117 /*u*/,  letters_118 /*v*/,  letters_119 /*w*/,  letters_120 /*x*/ };
*/

#endif

/* -----------------------------------------------------------------  */
/** Draws a figure (0-9). You should call it with set=1, 
 * wait a little them call it again with set=0
 * @param figure is the figure [0-9]
 * @param x,y coordinates, 
 * @param set is 1 or 0 to draw or clear it
 */
uint8_t Font::Draw(unsigned char letter,int x,int y,int set) {
  uint16_t maxx=0;

  uint8_t charCol;
  uint8_t charRow;

  prog_uchar* character;
  if (letter==' ') return 3+2;
  if (letter<fontMin || letter>fontMax) {
    return 0;
  }

//  if (letter>90) {
//     character = font2[letter-90];
//  } else {
    character = font[letter-fontMin];
//  }

  int i=0;

  charCol = pgm_read_byte_near(character);
  charRow = pgm_read_byte_near(character + 1);

  while (charRow!=9) {
    if (charCol>maxx) maxx=charCol;
    if (
     charCol + x <14 && 
     charCol + x >=0 && 
     charRow + y <9 && 
     charRow + y >=0
    ) {
        LedSign::Set(charCol + x, charRow+y, set);
    } 
    i+=2;

    charCol = pgm_read_byte_near(character + i);
    charRow = pgm_read_byte_near(character + 1 + i);
  }
  return maxx+2;
}


/* -----------------------------------------------------------------  */
/** Draw a figure in the other direction (rotated 90°)
 * @param figure is the figure [0-9]
 * @param x,y coordinates, 
 * @param set is 1 or 0 to draw or clear it
*/
uint8_t Font::Draw90(unsigned char letter,int x,int y,int set) {
  uint16_t maxx=0;

  uint8_t charCol;
  uint8_t charRow;

  prog_uchar* character;
  if (letter==' ') return 3+2;
  if (letter<fontMin || letter>fontMax) {
    return 0;
  }

//  if (letter>90) {
//     character = font2[letter-90];
//  } else {
    character = font[letter-fontMin];
//  }

  int i=0;

  charCol = pgm_read_byte_near(character);
  charRow = pgm_read_byte_near(character + 1);

  while (charRow!=9) {
    if (charCol>maxx) maxx=charCol;
    if (
     charRow + x <14 && 
     charRow + x >=0 && 
     charCol + y <8 && 
     charCol + y >=0
    ) {
        LedSign::Set(7 - charRow + x, charCol + y, set);
    } 
    i+=2;

    charCol = pgm_read_byte_near(character + i);
    charRow = pgm_read_byte_near(character + 1 + i);
  }
  return maxx+2;

}





