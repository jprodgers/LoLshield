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

#include "Charliplexing.h"      //initializes the LoL Sheild library

// Convenient 2D point structure
struct Point {
  float x;
  float y;
};



float phase = 0.0;
float phaseIncrement = 0.08;  // Controls the speed of the moving points. Higher == faster. I like 0.08 .
float colorStretch = 0.11;    // Higher numbers will produce tighter color bands. I like 0.11 .



// This function is called once, when the sketch starts.
void setup() {
  LedSign::Init(DOUBLE_BUFFER | GRAYSCALE);
}



// This function is called every frame.
void loop() {
  phase += phaseIncrement;
  
  // The two points move along Lissajious curves, see: http://en.wikipedia.org/wiki/Lissajous_curve
  // We want values that fit the LED grid: x values between 0..13, y values between 0..8 .
  // The sin() function returns values in the range of -1.0..1.0, so scale these to our desired ranges.
  // The phase value is multiplied by various constants; I chose these semi-randomly, to produce a nice motion.
  Point p1 = { (sin(phase*1.000)+1.0) * 7.5, (sin(phase*1.310)+1.0) * 4.0 };
  Point p2 = { (sin(phase*1.770)+1.0) * 7.5, (sin(phase*2.865)+1.0) * 4.0 };
  
  byte row, col;
  
  // For each row...
  for( row=0; row<9; row++ ) {
    float row_f = float(row);  // Optimization: Keep a floating point value of the row number, instead of recasting it repeatedly.
    
    // For each column...
    for( col=0; col<14; col++ ) {
      float col_f = float(col);  // Optimization.
      
      // Calculate the distance between this LED, and p1.
      Point dist1 = { col_f - p1.x, row_f - p1.y };  // The vector from p1 to this LED.
      float distance = dist1.x*dist1.x + dist1.y*dist1.y;
      
      // Calculate the distance between this LED, and p2.
      Point dist2 = { col_f - p2.x, row_f - p2.y };  // The vector from p2 to this LED.
      // Multiply this with the other distance, this will create weird plasma values :)
      distance *= dist2.x*dist2.x + dist2.y*dist2.y;
      distance = sqrt(distance);
      
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
  
  LedSign::Flip(true);
}

