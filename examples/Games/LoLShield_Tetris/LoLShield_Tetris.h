/*
  Tetris, an adaptation for LOL Shield for Arduino
  Copyright 2009/2010 Aurélien Couderc <acouderc@april.org>
  With the kind help and good ideas of Benjamin Sonntag <benjamin@sonntag.fr> http://benjamin.sonntag.fr/

  History:
  	2010-01-01 - V1.0 Initial version, at Berlin after 26C3 :D

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

/**
 * The coord struct holds an (x,y) pair, as used in the pieces declarations
 * an in the position structure.
 */
typedef struct coord {
  int8_t x;
  int8_t y;
} coord_t;

typedef struct coordPacked {
  unsigned x:2, y:2;
} coordPacked_t;

/**
 * One piece view. Each Tetris piece may have one to four views.
 */
typedef struct pieceView {
  coordPacked_t elements[4];
} pieceView_t;

/**
 * One Tetris piece object, made of one to four views.
 */
typedef struct piece {
  pieceView_t views[4];
} piece_t;

/**
 * Structure to hold the current position and view of the piece
 * being played.
 */
typedef struct pos {
  coord_t coord;
  uint8_t view;
} pos_t;
