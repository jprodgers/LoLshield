/*
  Figure drawing library

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

#include "Figure.h"
#include "Charliplexing.h"
#include <Arduino.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

#define C(c,r) ((c << 4) | r)

PROGMEM uint8_t figuresData[][14] = { 
{ C(0,0), C(1,0), C(2,0), C(0,1), C(2,1), C(0,2), C(2,2), C(0,3), C(2,3), C(0,4), C(1,4), C(2,4), 255, 255 },
{ C(1,0), C(0,1), C(1,1), C(1,2), C(1,3), C(0,4), C(1,4), C(2,4), 255, 255, 255, 255, 255, 255 },
{ C(0,0), C(1,0), C(2,0), C(2,1), C(1,2), C(0,3), C(0,4), C(1,4), C(2,4), 255, 255, 255, 255, 255 },
{ C(0,0), C(1,0), C(2,0), C(2,1), C(0,2), C(1,2), C(2,3), C(0,4), C(1,4), C(2,4), 255, 255, 255, 255 },
{ C(0,0), C(2,0), C(0,1), C(2,1), C(0,2), C(1,2), C(2,2), C(2,3), C(2,4), 255, 255, 255, 255, 255 },
{ C(0,0), C(1,0), C(2,0), C(0,1), C(0,2), C(1,2), C(2,2), C(2,3), C(0,4), C(1,4), C(2,4), 255, 255, 255 },
{ C(0,0), C(1,0), C(2,0), C(0,1), C(0,2), C(1,2), C(2,2), C(0,3), C(2,3), C(0,4), C(1,4), C(2,4), 255, 255 },
{ C(0,0), C(1,0), C(2,0), C(2,1), C(2,2), C(1,3), C(1,4), 255, 255, 255, 255, 255, 255, 255 },
{ C(0,0), C(1,0), C(2,0), C(0,1), C(2,1), C(0,2), C(1,2), C(2,2), C(0,3), C(2,3), C(0,4), C(1,4), C(2,4), 255 },
{ C(0,0), C(1,0), C(2,0), C(0,1), C(2,1), C(0,2), C(1,2), C(2,2), C(2,3), C(0,4), C(1,4), 255, 255, 255 }
};


/* -----------------------------------------------------------------  */
/** Draws a figure (0-9). You should call it with set=1, 
 * wait a little them call it again with set=0
 * @param figure is the figure [0-9]
 * @param x,y coordinates, 
 * @param set is 1 or 0 to draw or clear it
 */
void Figure::Draw(int figure,int x,int y,int set) {
  const uint8_t* character = figuresData[figure];
  for (;;) {
    uint8_t data = pgm_read_byte_near(character++);
    if (data == 255)
      break;
    uint8_t charCol = data >> 4, charRow = data & 15;
    if (
      charCol+x<DISPLAY_COLS && 
      charCol+x>=0 && 
      charRow+y<DISPLAY_ROWS && 
      charRow+y>=0
     ) {
         LedSign::Set(charCol+x,charRow+y,set);
     } 
  }
}


/* -----------------------------------------------------------------  */
/** Draw a figure in the other direction (rotated 90°)
 * You should call it with set=1, 
 * wait a little them call it again with set=0
 * @param figure is the figure [0-9]
 * @param x,y coordinates, 
 * @param set is 1 or 0 to draw or clear it
*/
void Figure::Draw90(int figure,int x,int y,int set) {
  const uint8_t* character = figuresData[figure];
  for (;;) {
    uint8_t data = pgm_read_byte_near(character++);
    if (data == 255)
      break;
    uint8_t charCol = data >> 4, charRow = data & 15;
    if (
      (5-charRow)+x<DISPLAY_COLS && 
      (5-charRow)+x>=0 && 
      charCol+y<DISPLAY_ROWS && 
      charCol+y>=0
     ) {
         LedSign::Set((5-charRow)+x,charCol+y,set);
     } 
  }
}


/* -----------------------------------------------------------------  */
/** Scroll a number from right to left
 * remove unused figures (0 at the left)
 * valid for up to 7 figures.
 * @param value is the value to draw and scroll
 * @param x is the coordinate where we put the top of the figure [0-13]
*/
void Figure::Scroll90(unsigned long value,uint8_t x) {
  int i,j,k;
  uint8_t figures[]={ 
    (value%10000000)/1000000, 
    (value%1000000)/100000, 
    (value%100000)/10000, 
    (value%10000)/1000,
    (value%1000)/100,
    (value%100)/10,
    (value%10)
  };
  j=0;
  while (figures[j]==0 && j<6) j++;
  
  for(i=0;i<9+(7-j)*5;i++) {
    for(k=j;k<=6;k++)
      Figure::Draw90(figures[k],3,-i+9+4*(k-j) ,1);
    delay(100);
    for(k=j;k<=6;k++)
      Figure::Draw90(figures[k],3,-i+9+4*(k-j) ,0);
  }
}


