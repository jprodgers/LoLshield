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

#include "Charliplexing.h"
#include "Figure.h"
#include "LoLShield_Tetris.h"

/** The current level. */
int level;

/** The score of the user (number of points = speed of each killed ennemy - number of ennemies missed) */
int score;

/** Number of lines cleared at current level. */
byte linesCleared;

/** The game grid size. */
const uint8_t GRID_HEIGHT = 14;
const uint8_t GRID_WIDTH = 9;

boolean playGrid[GRID_HEIGHT][GRID_WIDTH];

const piece_t pieces[7] = {
  {{
    // The single view of the square piece :
    // 00
    // 00
    {{{1,1}, {2,1}, {1,2}, {2,2}}},
    {{{1,1}, {2,1}, {1,2}, {2,2}}},
    {{{1,1}, {2,1}, {1,2}, {2,2}}},
    {{{1,1}, {2,1}, {1,2}, {2,2}}},
  }},
  {{
    // The two views of the bar piece :
    // 0000
    {{{0,1}, {1,1}, {2,1}, {3,1}}},
    {{{2,0}, {2,1}, {2,2}, {2,3}}},
    {{{0,1}, {1,1}, {2,1}, {3,1}}},
    {{{2,0}, {2,1}, {2,2}, {2,3}}},
  }},
  {{
    // The two views of the first S :
    // 00
    //  00
    {{{0,1}, {1,1}, {1,2}, {2,2}}},
    {{{1,1}, {1,2}, {2,0}, {2,1}}},
    {{{0,1}, {1,1}, {1,2}, {2,2}}},
    {{{1,1}, {1,2}, {2,0}, {2,1}}},
  }},
  {{
    // The two views of the second S :
    //  00
    // 00
    {{{0,2}, {1,1}, {1,2}, {2,1}}},
    {{{0,0}, {0,1}, {1,1}, {1,2}}},
    {{{0,2}, {1,1}, {1,2}, {2,1}}},
    {{{0,0}, {0,1}, {1,1}, {1,2}}},
  }},
  {{
    // The four views of the first L :
    // 000
    // 0
    {{{0,1}, {0,2}, {1,1}, {2,1}}},
    {{{0,0}, {1,0}, {1,1}, {1,2}}},
    {{{0,1}, {1,1}, {2,0}, {2,1}}},
    {{{1,0}, {1,1}, {1,2}, {2,2}}},
  }},
  {{
    // The four views of the second L :
    // 000
    //   0
    {{{0,1}, {1,1}, {2,1}, {2,2}}},
    {{{1,0}, {1,1}, {1,2}, {2,0}}},
    {{{0,0}, {0,1}, {1,1}, {2,1}}},
    {{{0,2}, {1,0}, {1,1}, {1,2}}},
  }},
  {{
    // The four views of the T :
    // 000
    //  0
    {{{0,1}, {1,1}, {1,2}, {2,1}}},
    {{{1,0}, {1,1}, {1,2}, {2,1}}},
    {{{0,1}, {1,0}, {1,1}, {2,1}}},
    {{{0,1}, {1,0}, {1,1}, {1,2}}},
  }},
};

const int levelMultiplier[] = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
const int linesMultiplier[] = {100, 400, 900, 2000};

/** The piece being played. */
const piece_t* currentPiece;
/** The current position and view of the piece being played. */
pos_t position;

/* -----------------------------------------------------------------  */
/** Switches on or off the display of a Tetris piece.
 * @param piece the piece to be displayed or removed.
 * @param position the position and view of the piece to draw or remove.
 * @param set 1 or 0 to draw or remove the piece.
 */
void switchPiece(const piece_t* piece, const pos_t& position, uint8_t c=1) {
  for(uint8_t i=0;i<4;i++) {
    coordPacked_t element = piece->views[position.view].elements[i];
    uint8_t eltXPos = element.x+position.coord.x;
    uint8_t eltYPos = element.y+position.coord.y;
    LedSign::Set(13-eltYPos, eltXPos, c);
  }
}


/* -----------------------------------------------------------------  */
/**
 * Redraw a section of the tetris play grid between the given
 * indexes (included).
 * @param top the index of the top line of the section to redraw.
 * @param bottom the index the bottom line of the section to redraw.
 *               This parameter MUST be greater or equal than top.
 */
void redrawLines(uint8_t top, uint8_t bottom) {
  for (uint8_t y=top; y<=bottom; y++)
    for (uint8_t x=0; x<GRID_WIDTH; x++)
      LedSign::Set(13-y,x,playGrid[y][x]);
}


/* -----------------------------------------------------------------  */
/**
 * End of the game, draw the score using a scroll.
 */
void endGame() {
  for (uint8_t y=0;y<=13;y++) {
    LedSign::Vertical(13-y, 0);
    delay(100);
  }
  // Draw the score and scroll it
  Figure::Scroll90(score);
}

/* -----------------------------------------------------------------  */
/**
 * Game initialization, or reinitialization after a game over.
 */
void startGame() {
  // Initialize variables.
  level = 0;
  score = 0;
  linesCleared = 0;
  memset(playGrid, '\0', sizeof(playGrid)); 
  LedSign::Clear();
}

/* -----------------------------------------------------------------  */
/**
 * Test whether a given piece can be put at a given location in the
 * given location. This is used to check all piece moves.
 * @param piece the piece to try and put on the play grid.
 * @param position the position and view to try and put the piece.
 */
boolean checkPieceMove(const piece_t* piece, const pos_t& position) {
  for (uint8_t i=0; i<4; i++) {
    coordPacked_t element = piece->views[position.view].elements[i];
    int8_t eltXPos = element.x+position.coord.x;
    int8_t eltYPos = element.y+position.coord.y;
    // Check x boundaries.
    if (eltXPos>8 || eltXPos<0)
      return false;
    // Check y boundaries.
    if (eltYPos>13 || eltYPos<0)
      return false;
    // Check collisions in grid.
    if (playGrid[eltYPos][eltXPos])
      return false;
  }
  
  return true;
}

/* -----------------------------------------------------------------  */
/**
 * Handle player actions : left/right move and piece rotation.
 */
void playerMovePiece()
{
  boolean moveSuccess;
  int inputPos;
  pos_t newPos;

  // First try rotating the piece if requested.
  // Ensure the player released the rotation button before doing a second one.
  static byte status=0;
  if (status == 0) {
    if (analogRead(4)>1000) {
      status = 1;
      newPos = position;
      newPos.view = (newPos.view+1)&3;
      moveSuccess = checkPieceMove(currentPiece, newPos);
      if (moveSuccess) {
        switchPiece(currentPiece, position, 0);
        switchPiece(currentPiece, newPos, 1);
        position = newPos;
      }
    }
  } else {
    if (analogRead(4)<1000) {
      status = 0;
    }
  }

  newPos = position;
  // Tweak the analog input to get a playable input.
  inputPos=8-(analogRead(5)/72); // we reverse the command (variator wrongly connected ;) )
  if (inputPos != position.coord.x) {
    // Try moving the piece to the requested position.
    // We must do it step by step and leave the loop at the first impossible movement otherwise we might
    // traverse pieces already in the game grid if the user changes the input fast between two game loop iterations.
    int diffSign = (inputPos>position.coord.x) ? 1 : -1;
    for(int i=position.coord.x;(inputPos-i)*diffSign>=0;i+=diffSign) {
      newPos.coord.x = i;
      if (i<-1 || i>8) {
        // Skip out of screen iterations.
        // Don't skip -2, it's a correct x position for certain pieces' views.
        continue;
      } else {
        moveSuccess = checkPieceMove(currentPiece, newPos);
        if (moveSuccess) {
          switchPiece(currentPiece, position, 0);
          switchPiece(currentPiece, newPos, 1);
          position = newPos;
        } else {
          break;
        }
      }
    }
  }
}


/* -----------------------------------------------------------------  */
/**
 * Handle the current piece going down every few game loop iterations.
 * @param count game loop counter, used to know if the piece should go
 *              down at each call.
 */
void timerPieceDown(uint32_t& count) {
  // Every 10-level iterations, make the piece go down.
  // TODO The level change code is largely untested and surely needs tweaking.
  if (++count % (10-level) == 0) {
    pos_t newPos = position;
    newPos.coord.y++;

    boolean moveSuccess = checkPieceMove(currentPiece, newPos);
    if (moveSuccess) {
      switchPiece(currentPiece, position, 0);
      switchPiece(currentPiece, newPos, 1);
      position = newPos;
    } else {
      // Drop the piece on the grid.
      for (uint8_t i=0; i<4; i++) {
        coordPacked_t element = currentPiece->views[position.view].elements[i];
        uint8_t eltXPos = element.x+position.coord.x;
        uint8_t eltYPos = element.y+position.coord.y;
        playGrid[eltYPos][eltXPos] = true;
      }
 
      processEndPiece();
      nextPiece();
    }
  }
}


/* -----------------------------------------------------------------  */
/**
 * Handles :
 * - the dropping of the current piece on the game grid when it can't
 *   go lower ;
 * - the detection and removal of full lines ;
 * - the score update ;
 * - the level update when needed.
 */
void processEndPiece() {
  uint8_t fullLines[4];
  uint8_t numFull = 0;
  for (int8_t y=13; y>=0; y--) {
    boolean full = true;
    for (uint8_t x=0; x<9; x++)
      if (!playGrid[y][x])
	full = false;
    if (full)
      fullLines[numFull++] = y;
  }
  
  if (numFull) {
    // Blink full lines.
    for (uint8_t i=0; i<5; i++) {
      for (uint8_t j=0; j<numFull; j++)
        LedSign::Vertical(13-fullLines[j], i&1);
      delay(150);
    }
 
    // Remove full lines from the array.
    for (uint8_t i=0; i<numFull; i++) {
      uint8_t lineIdx = fullLines[i];
      // Move all lines above one step down.
      for (uint8_t j=lineIdx; j>0; j--)
        memcpy(playGrid+j, playGrid+j-1, GRID_WIDTH*sizeof(boolean));
      memset(playGrid, '\0', GRID_WIDTH*sizeof(boolean));

      // Update the indexes of the other lines to remove.
      for (uint8_t k=i; k<numFull; k++)
        fullLines[k]++;
    }

    // Update the display.
    redrawLines(0, fullLines[0]);

    // Sega scoring algorithm.
    score += levelMultiplier[level] * linesMultiplier[numFull-1];
  
    // Level update.
    linesCleared += numFull;
    if (linesCleared >= 4) {
      linesCleared = 0;
      level++;
    }
  }
}

/* -----------------------------------------------------------------  */
/**
 * Start dropping a new randomly chosen piece.
 */
void nextPiece() {
  currentPiece = &pieces[random(0,7)];
  
  position.coord.x = 3;
  position.coord.y = -1;
  position.view = 0;

  if (!checkPieceMove(currentPiece, position)) {
    endGame();
    startGame();
  }
}


/* -----------------------------------------------------------------  */
/** MAIN program Setup
 */
void setup()                    // run once, when the sketch starts
{
  LedSign::Init();
  randomSeed(analogRead(2));
  startGame();
  nextPiece();
}


/* -----------------------------------------------------------------  */
/** MAIN program Loop
 */
void loop()                     // run over and over again
{
  static uint32_t counter = 0;

  playerMovePiece();
  timerPieceDown(counter);
  delay(40);
}
