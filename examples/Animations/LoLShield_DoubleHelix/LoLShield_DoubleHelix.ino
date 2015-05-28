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
 
#include "Charliplexing.h"      //initializes the LoL Sheild library



/////////////////////////////////////////////////////////////////////////////
//
// You can tweak these values to create a custom DNA molecule :)
//

const float stretch = 0.44;           // The width of each sine wave. Smaller values create wider sine waves. I like 0.44 .

const float phaseIncrement = 0.1;     // How fast the sines move. I like 0.1 .

// The phase of the "darker" sine wave will drift (relative to the "lighter" sine wave).
// This makes the DoubleHelix more organic/hypnotic .
const float driftIncrement = 0.019;   // The speed it drifts back and forth. Larger == faster. I like 0.019 .
const float driftForce = 0.4;         // The visual amount of drift. I like 0.4 .

// On every other column, light the LEDs between the sine waves, resembling the nucleotides of a DNA molecule.
// This looks good if we switch between lighting odd columns, then even columns -- the molecule appears to be moving.
const float barPhaseIncrement = 0.09; // Bar movement speed. Plz use values between 0..1 . I like 0.09 .

// Brightness values. Range is 0..7
const byte lightSineBrightness = 7;
const byte darkSineBrightness = 3;
const byte barBrightness = 1;

// (End tweak section)
/////////////////////////////////////////////////////////////////////////////



// These values change every frame:
float phase = 0.0;              // This is how "far" we've travelled along the DNA.
float driftPhase = 0.0; 
float barPhase = 0.0;           



// This function is called once, when the sketch starts.
void setup() {
  LedSign::Init(DOUBLE_BUFFER | GRAYSCALE); // Initializes a grayscale frame buffer.
}



// This function is called every frame.
void loop() {
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
      } else if( row==darkSine ) {
        LedSign::Set( col, row, darkSineBrightness );  // The third argument is the brightness.
        
      } else {
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
          } else if( darkSine < lightSine ) {
            if( darkSine<row && row<lightSine ) {
              color = barBrightness;
            }
          }
        }
        
        LedSign::Set( col, row, color );
      }
    }
  }
  
  LedSign::Flip(true);
}
