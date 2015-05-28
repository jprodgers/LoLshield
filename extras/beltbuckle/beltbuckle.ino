//This is a series of animations for a belt buckle. You can cycle the animations by cycling the power.

#include "Charliplexing.h"
#include "Myfont.h"
#include "Arduino.h"
#include <EEPROM.h>

int toggleState;
int EEPROMaddress = 0;
int charLength[]={
  20, 14, 23, 30};
unsigned char text0[]="My eyes are up there";
unsigned char text1[]="Blinky or GTFO";
unsigned char text2[]="Enjoying the lightshow?";
unsigned char text3[]="Would you like to play a game?";

//Game of Life stuff
#define DELAY 150             //Sets the time each generation is shown
#define RESEEDRATE 5000       //Sets the rate the world is re-seeded
#define SIZEX 14              //Sets the X axis size
#define SIZEY 9               //Sets the Y axis size
byte world[SIZEX][SIZEY][2];  //Creates a double buffer world
long density = 50;            //Sets density % during seeding
int geck = 0;                 //Counter for re-seeding

//ball stuff
int collision[14][9];

void setup(){
  toggleState = EEPROM.read(EEPROMaddress);
  upToggleState();

  if (1==toggleState || 7==toggleState){
    LedSign::Init(GRAYSCALE);
  }
  else{
    LedSign::Init();
  }

  for (int i = toggleState+1; i > 0; i--){
    LedSign::Set(i-1, 0, 255);
  }
  delay(1000);
  LedSign::Clear(0);

}

void loop(){
  /*
 0 "My eyes are up there"
   1 *Plasma
   2 "Blinky or GTFO"
   3 *Game of Life
   4 "Enjoying the lightshow?"
   5 *Balls
   6 "Would you like to play a game?"
   7 *Double Helix
   */
  switch(toggleState){
  case 0:
    Myfont::Banner(charLength[0],text0);
    break;
  case 1:
    plasma();
    break;
  case 2:
    Myfont::Banner(charLength[1],text1);
    break;
  case 3:
    life();
  case 4:
    Myfont::Banner(charLength[2],text2);
    break;
  case 5:
    balls();
    break;
  case 6:
    Myfont::Banner(charLength[3],text3);
    break;
  case 7:
    DNA();
    break;
  default:
    EEPROM.write(EEPROMaddress, 0);
  }
}

//Ups or resets the state counter
void upToggleState(){
  toggleState++;
  if (toggleState > 7) toggleState = 0;
  EEPROM.write(EEPROMaddress, toggleState);
}

void plasma(){
  /*
  Plasma
   written by Zach Archer http://zacharcher.com/
   
   NOTES:
   - Requires the LoLshield library to run. Get the library here: http://code.google.com/p/lolshield/downloads/
   - How to install the library: http://www.arduino.cc/en/Hacking/Libraries
   
   This sketch moves two points along Lissajious curves. See: http://en.wikipedia.org/wiki/Lissajous_curve
   The distances between each LED and each point are multiplied,
   then this value is shaped using a sine function, and this sets the brightness of each LED.
   */

  // Convenient 2D point structure
  struct Point {
    float x;
    float y;
  };



  float phase = 0.0;
  float phaseIncrement = 0.08;  // Controls the speed of the moving points. Higher == faster. I like 0.08 .
  float colorStretch = 0.11;    // Higher numbers will produce tighter color bands. I like 0.11 .


  // This function is called every frame.
  while(true) {
    phase += phaseIncrement;

    // The two points move along Lissajious curves, see: http://en.wikipedia.org/wiki/Lissajous_curve
    // We want values that fit the LED grid: x values between 0..13, y values between 0..8 .
    // The sin() function returns values in the range of -1.0..1.0, so scale these to our desired ranges.
    // The phase value is multiplied by various constants; I chose these semi-randomly, to produce a nice motion.
    Point p1 = { 
      (sin(phase*1.000)+1.0) * 7.5, (sin(phase*1.310)+1.0) * 4.0                 };
    Point p2 = { 
      (sin(phase*1.770)+1.0) * 7.5, (sin(phase*2.865)+1.0) * 4.0                 };

    byte row, col;

    // For each row...
    for( row=0; row<9; row++ ) {
      float row_f = float(row);  // Optimization: Keep a floating point value of the row number, instead of recasting it repeatedly.

      // For each column...
      for( col=0; col<14; col++ ) {
        float col_f = float(col);  // Optimization.

        // Calculate the distance between this LED, and p1.
        Point dist1 = { 
          col_f - p1.x, row_f - p1.y                                 };  // The vector from p1 to this LED.
        float distance = sqrt( dist1.x*dist1.x + dist1.y*dist1.y );

        // Calculate the distance between this LED, and p2.
        Point dist2 = { 
          col_f - p2.x, row_f - p2.y                                 };  // The vector from p2 to this LED.
        // Multiply this with the other distance, this will create weird plasma values :)
        distance *= sqrt( dist2.x*dist2.x + dist2.y*dist2.y );
        //distance += sqrt( dist2.x*dist2.x + dist2.y*dist2.y );  // Variation: weird linear color bands. Might need to increase colorStretch


        // Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
        // You can use a cos() for slightly different shading, or experiment with other functions. Go crazy!
        float color_f = (sin( distance * colorStretch ) + 1.0) * 0.5;  // range: 0.0...1.0

        // Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
        color_f *= color_f;
        //color_f *= color_f*color_f*color_f;  // Uncomment this line to make it even darker :)

        // Scale the color up to 0..7 . Max brightness is 7.
        LedSign::Set( col, row, byte( round(color_f * 7.0) ) );
      }
    }
    // There's so much math happening, it's already a bit slow ;) No need for extra delays!
    //delay( 20 );
  }
}

void DNA(){
  /*
  DoubleHelix
   written by Zach Archer http://zacharcher.com/
   
   NOTES:
   - Requires the LoLshield library to run. Get the library here: http://code.google.com/p/lolshield/downloads/
   - How to install the library: http://www.arduino.cc/en/Hacking/Libraries
   
   This sketch draws two sine waves with different brightness values.
   The phase of the "darker" sine wave will drift a bit.
   On every other column, LEDs between the sines will be subtly lit (hopefully resembling DNA nucleobases).
   */

  // You can tweak these values to create a custom DNA molecule :)
  float stretch = 0.44;           // The width of each sine wave. Smaller values create wider sine waves. I like 0.44 .

  float phaseIncrement = 0.1;     // How fast the sines move. I like 0.1 .

  // The phase of the "darker" sine wave will drift (relative to the "lighter" sine wave).
  // This makes the DoubleHelix more organic/hypnotic .
  float driftIncrement = 0.019;   // The speed it drifts back and forth. Larger == faster. I like 0.019 .
  float driftForce = 0.4;         // The visual amount of drift. I like 0.4 .

  // On every other column, light the LEDs between the sine waves, resembling the nucleotides of a DNA molecule.
  // This looks good if we switch between lighting odd columns, then even columns -- the molecule appears to be moving.
  float barPhaseIncrement = 0.09; // Bar movement speed. Plz use values between 0..1 . I like 0.09 .

  // Brightness values. Range is 0..7
  byte lightSineBrightness = 7;
  byte darkSineBrightness = 3;
  byte barBrightness = 1;

  // (End tweak section)

  // These values change every frame:
  float phase = 0.0;              // This is how "far" we've travelled along the DNA.
  float driftPhase = 0.0; 
  float barPhase = 0.0;           

  // This function is called every frame.
  while(true) {
    phase += phaseIncrement;  // Move the sine waves forward.

    // The "darker" sine wave drifts (relative to the "lighter" sine wave).
    driftPhase += driftIncrement;

    // Increment the position of the bars.
    barPhase += barPhaseIncrement;
    if( barPhase > 1.0 ) barPhase -= 1.0;  // Wrap this value between 0..1 .

    // We'll hilite either the even columns, or odd columns, depending on the value of barPhase.
    boolean drawEvenBars = (barPhase < 0.5);

    byte row, col;

    // For each column of LEDs...
    for( col=0; col<14; col++ ) {

      // This is the "raw" value for the lighter sine wave. Range: -1.0...1.0
      float lightSineThisColumn = sin( phase + float(col)*stretch );

      // Scale the "raw" value and round it off, so the range is 0..8 . This is the LED we're going to light in this column.
      int lightSine = int( round( lightSineThisColumn*4.0 ) ) + 4;

      // driftPhase controls the phase drift of the "darker" sine.
      // The drift amount is derived from this sin() function, so it will drift back and forth.
      // Orbit around 2.1, which is about 1/3 phase offset from the lighter sine wave (2*PI/3). Looks pretty good.
      float drift = 2.1 + (driftForce * sin( driftPhase ));

      // This is the LED we're going to light for the "dark" sine wave.
      // This is similar to computing the lightSine value, but it's compacted into one line :P
      int darkSine = int( round( sin(phase+drift+float(col)*stretch)*4.0 ) ) + 4;

      // For each LED within the column...
      for( row=0; row<9; row++ ) {

        // Does this LED belong to our light sine wave?      
        if( row==lightSine ) {
          LedSign::Set( col, row, lightSineBrightness );  // The third argument is the brightness. Max bright == 7.

          // Does this LED belong to our dark sine wave?
        } 
        else if( row==darkSine ) {
          LedSign::Set( col, row, darkSineBrightness );  // The third argument is the brightness.

        } 
        else {
          // This LED doesn't belong to either sine wave. So we'll turn it off, unless it belongs to a vertical bar.
          int color = 0;  // 0 == unlit

          // Alternate even/odd columns:
          //   If col is an odd number, (col & 0x1) evaluates to true. (Example: 13 == B1101, rightmost bit is 1, so it's odd!)
          //   The ^ operator is binary XOR. So this statement evaluates true if _one_ condition is met, but _not_ both.
          if( (col & 0x1) ^ (drawEvenBars) ) {

            // If lightSine is above this LED, and darkSine is below, then this LED belongs to a vertical bar.
            if( lightSine < darkSine ) {
              if( lightSine<row && row<darkSine ) {
                color = barBrightness;
              }

              // If darkSine is above, and lightSine is below, this LED belongs to a vertical bar.
            } 
            else if( darkSine < lightSine ) {
              if( darkSine<row && row<lightSine ) {
                color = barBrightness;
              }
            }
          }

          LedSign::Set( col, row, color );
        }
      }
    }

    // Wait between frames to slow down the animation.
    delay( 20 );
  }
}

void balls(){

  //0 = xPos, 1 = xDir, 2 = yPos, 3 = yDir
  /*int balls [][4] = {
   {0,1,8,1},
   {1,1,7,1},
   {2,1,6,1}, 
   {3,1,5,1}, 
   {4,1,4,1},
   {5,1,3,1}, 
   {6,1,2,1}, 
   {7,1,1,1}, 
   {8,1,0,1},
   {1,1,1,1},
   {255}};
   */
  int balls [][4] = {
    {
      7,1,0,1    }
    ,
    {
      6,1,1,1    }
    ,
    {
      8,1,1,1    }
    ,
    {
      5,1,2,1    }
    ,
    {
      9,1,2,1    }
    ,
    {
      4,1,3,1    }
    ,
    {
      10,1,3,1    }
    ,
    {
      5,1,4,1    }
    ,
    {
      9,1,4,1    }
    ,
    {
      6,1,5,1    }
    ,
    {
      8,1,5,1    }
    ,
    {
      7,1,6,1    }
    ,
    {
      255    }
  };


  int numBalls;  
  int scrollSpeed = 100;      //delay between frames

  int collision[14][9];
  //int clearedCollision[14][9];

  LedSign::Init(); //initializes a grayscale frame buffer
  for (numBalls = 0; numBalls < 255; numBalls++){
    if(balls[numBalls][0] == 255) break;
  }

  while(true)                 // run over and over again
  { 
    for (int i = 0; i < numBalls; i++)moveBall(balls[i]);
    delay(scrollSpeed);
    LedSign::Clear(0);
    for (int x = 0; x < 14; x++) for (int y = 0; y <9; y++) collision[x][y] = 0;
  }

}
void moveBall(int ball[])
{
  //0 = xPos, 1 = Dir, 2 = yPos, 3 = yDir
  if (ball[0] == 13)ball[1] = 0;
  if (ball[0] == 0)ball[1] = 1;
  if (ball[2] == 8)ball[3] = 0;
  if (ball[2] == 0)ball[3] = 1;

  if ((ball[1] == 1) && (collision[ball[0]+1][ball[2]]==1)) ball[1]= !ball[1];
  if ((ball[1] == 0) && (collision[ball[0]-1][ball[2]]==1)) ball[1]= !ball[1];
  if ((ball[3] == 1) && (collision[ball[0]][ball[2]+1]==1)) ball[3]= !ball[3];
  if ((ball[3] == 0) && (collision[ball[0]][ball[2]-1]==1)) ball[3]= !ball[3];

  if (ball[1]) ball[0]++;
  else ball[0]--;

  if (ball[3]) ball[2]++;
  else ball[2]--;
  collision[ball[0]][ball[2]] = 1;  
  LedSign::Set(ball[0], ball[2], 1);  
}

void life(){
  /*
 Conway's "Life"
   
   Writen for the LoL Shield, designed by Jimmie Rodgers:
   http://jimmieprodgers.com/kits/lolshield/
   
   This needs the Charliplexing library, which you can get at the
   LoL Shield project page: http://code.google.com/p/lolshield/
   
   Created by Jimmie Rodgers on 12/30/2009.
   Adapted from: http://www.arduino.cc/playground/Main/DirectDriveLEDMatrix
   
   History:
   	December 30, 2009 - V1.0 first version written at 26C3/Berlin
   
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

  //#include <Charliplexing.h>    //Imports the library, which needs to be
  //Initialized in setup.


  randomSeed(analogRead(5));
  //Builds the world with an initial seed.
  for (int i = 0; i < SIZEX; i++) {
    for (int j = 0; j < SIZEY; j++) {
      if (random(100) < density) {
        world[i][j][0] = 1;
      }
      else {
        world[i][j][0] = 0;
      }
      world[i][j][1] = 0;
    }
  }


  while(true) {
    // Birth and death cycle 
    for (int x = 0; x < SIZEX; x++) { 
      for (int y = 0; y < SIZEY; y++) {
        // Default is for cell to stay the same
        world[x][y][1] = world[x][y][0];
        int count = neighbours(x, y); 
        geck++;
        if (count == 3 && world[x][y][0] == 0) {
          // A new cell is born
          world[x][y][1] = 1; 
          LedSign::Set(x,y,1);
        } 
        else if ((count < 2 || count > 3) && world[x][y][0] == 1) {
          // Cell dies
          world[x][y][1] = 0;
          LedSign::Set(x,y,0);
        }
      }

    }

    //Counts and then checks for re-seeding
    //Otherwise the display will die out at some point
    geck++;
    if (geck > RESEEDRATE){
      seedWorld();
      geck = 0;
    }

    // Copy next generation into place
    for (int x = 0; x < SIZEX; x++) { 
      for (int y = 0; y < SIZEY; y++) {
        world[x][y][0] = world[x][y][1];
      }
    }
    delay(DELAY);
  }

  //Re-seeds based off of RESEEDRATE


  //Runs the rule checks, including screen wrap

}
void seedWorld(){
  randomSeed(analogRead(5));
  for (int i = 0; i < SIZEX; i++) {
    for (int j = 0; j < SIZEY; j++) {
      if (random(100) < density) {
        world[i][j][1] = 1;
      }
    } 
  }
}
int neighbours(int x, int y) {
  return world[(x + 1) % SIZEX][y][0] + 
    world[x][(y + 1) % SIZEY][0] + 
    world[(x + SIZEX - 1) % SIZEX][y][0] + 
    world[x][(y + SIZEY - 1) % SIZEY][0] + 
    world[(x + 1) % SIZEX][(y + 1) % SIZEY][0] + 
    world[(x + SIZEX - 1) % SIZEX][(y + 1) % SIZEY][0] + 
    world[(x + SIZEX - 1) % SIZEX][(y + SIZEY - 1) % SIZEY][0] + 
    world[(x + 1) % SIZEX][(y + SIZEY - 1) % SIZEY][0]; 
}
