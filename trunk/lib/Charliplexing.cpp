/*
  Charliplexing.cpp - Using timer2 with 1ms resolution
  
  Alex Wenger <a.wenger@gmx.de> http://arduinobuch.wordpress.com/
  Matt Mets <mahto@cibomahto.com> http://cibomahto.com/
  
  Timer init code from MsTimer2 - Javier Valencia <javiervalencia80@gmail.com>
  Misc functions from Benjamin Sonnatg <benjamin@sonntag.fr>
  
  History:
    2009-12-30 - V0.0 wrote the first version at 26C3/Berlin
    2010-01-01 - V0.1 adding misc utility functions 
      (Clear, Vertical,  Horizontal) comment are Doxygen complaints now
    2010-05-27 - V0.2 add double-buffer mode
    2010-08-18 - V0.9 Merge brightness and grayscale

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <inttypes.h>
#include <math.h>
#include <avr/interrupt.h>
#include "Charliplexing.h"

volatile unsigned int LedSign::tcnt2;


struct videoPage {
    uint8_t pixels[24*(SHADES-1)];
}; 

/* -----------------------------------------------------------------  */
/** Table for the LED multiplexing cycles
 * Each frame is made of 24 bytes (for the 24 display cycles)
 * There are SHADES frames per buffer in grayscale mode (one for each brigtness)
 * and twice that many to support double-buffered grayscale.
 */
videoPage leds[2];

/// Determines whether the display is in single or double buffer mode
uint8_t displayMode = SINGLE_BUFFER;

/// Flag indicating that the display page should be flipped as soon as the
/// current frame is displayed
volatile boolean videoFlipPage = false;

/// Pointer to the buffer that is currently being displayed
videoPage* displayBuffer;
uint8_t* displayPointer;

/// Pointer to the buffer that should currently be drawn to
videoPage* workBuffer;

/// Flag indicating that the timer buffer should be flipped as soon as the
/// current frame is displayed
volatile boolean videoFlipTimer = false;


// Timer counts to display each page for, plus off time
typedef struct timerInfo {
    uint8_t counts[SHADES];
    uint8_t prescaler[SHADES];
};

// Double buffer the timing information, of course.
timerInfo* frontTimer;
timerInfo* backTimer;

timerInfo* tempTimer;

timerInfo timer[2];

// Record a slow and fast prescaler for later use
typedef struct prescalerInfo {
    uint8_t relativeSpeed;
    uint8_t TCCR2;
};

// TODO: Generate these based on processor type and clock speed
prescalerInfo slowPrescaler = {1, 0x03};
//prescalerInfo fastPrescaler = {32, 0x01};
prescalerInfo fastPrescaler = {4, 0x02};

static bool initialized = false;

/// Uncomment to set analog pin 5 high during interrupts, so that an
/// oscilloscope can be used to measure the processor time taken by it
#define MEASURE_ISR_TIME
#ifdef MEASURE_ISR_TIME
uint8_t statusPIN = 19;
#endif

typedef struct LEDPosition {
    uint8_t mask;
    uint8_t cycle;
};


/* -----------------------------------------------------------------  */
/** Table for LED Position in leds[] ram table
 */

#define L(high, low)	{ _BV(high & 7), (low - 2) + ((high > 7) ? 12 : 0) }
const LEDPosition ledMap[126] = {
    L(13, 5), L(13, 6), L(13, 7), L(13, 8), L(13, 9), L(13,10), L(13,11), L(13,12),
    L(13, 4), L( 4,13), L(13, 3), L( 3,13), L(13, 2), L( 2,13),
    L(12, 5), L(12, 6), L(12, 7), L(12, 8), L(12, 9), L(12,10), L(12,11), L(12,13),
    L(12, 4), L( 4,12), L(12, 3), L( 3,12), L(12, 2), L( 2,12),
    L(11, 5), L(11, 6), L(11, 7), L(11, 8), L(11, 9), L(11,10), L(11,12), L(11,13),
    L(11, 4), L( 4,11), L(11, 3), L( 3,11), L(11, 2), L( 2,11),
    L(10, 5), L(10, 6), L(10, 7), L(10, 8), L(10, 9), L(10,11), L(10,12), L(10,13),
    L(10, 4), L( 4,10), L(10, 3), L( 3,10), L(10, 2), L( 2,10),
    L( 9, 5), L( 9, 6), L( 9, 7), L( 9, 8), L( 9,10), L( 9,11), L( 9,12), L( 9,13),
    L( 9, 4), L( 4, 9), L( 9, 3), L( 3, 9), L( 9, 2), L( 2, 9),
    L( 8, 5), L( 8, 6), L( 8, 7), L( 8, 9), L( 8,10), L( 8,11), L( 8,12), L( 8,13),
    L( 8, 4), L( 4, 8), L( 8, 3), L( 3, 8), L( 8, 2), L( 2, 8),
    L( 7, 5), L( 7, 6), L( 7, 8), L( 7, 9), L( 7,10), L( 7,11), L( 7,12), L( 7,13),
    L( 7, 4), L( 4, 7), L( 7, 3), L( 3, 7), L( 7, 2), L( 2, 7),
    L( 6, 5), L( 6, 7), L( 6, 8), L( 6, 9), L( 6,10), L( 6,11), L( 6,12), L( 6,13),
    L( 6, 4), L( 4, 6), L( 6, 3), L( 3, 6), L( 6, 2), L( 2, 6),
    L( 5, 6), L( 5, 7), L( 5, 8), L( 5, 9), L( 5,10), L( 5,11), L( 5,12), L( 5,13),
    L( 5, 4), L( 4, 5), L( 5, 3), L( 3, 5), L( 5, 2), L( 2, 5),
};
#undef L(high, low)


/* -----------------------------------------------------------------  */
/** Constructor : Initialize the interrupt code. 
 * should be called in setup();
 */
void LedSign::Init(uint8_t mode)
{
#ifdef MEASURE_ISR_TIME
    pinMode(statusPIN, OUTPUT);
    digitalWrite(statusPIN, LOW);
#endif

	float prescaler = 0.0;
	
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TIMSK2 &= ~(1<<TOIE2);
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
	TCCR2B &= ~(1<<WGM22);
	ASSR &= ~(1<<AS2);
	TIMSK2 &= ~(1<<OCIE2A);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2B |= ((1<<CS21) | (1<<CS20));
		TCCR2B &= ~(1<<CS22);
		prescaler = 32.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2B |= (1<<CS21);
		TCCR2B &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2B |= (1<<CS22);
		TCCR2B &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	}
#elif defined (__AVR_ATmega8__)
	TIMSK &= ~(1<<TOIE2);
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	TIMSK &= ~(1<<OCIE2);
	ASSR &= ~(1<<AS2);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2 |= ((1<<CS22) && (1<<CS20));
		TCCR2 &= ~(1<<CS21);
		prescaler = 128.0;
	}
#elif defined (__AVR_ATmega128__)
	TIMSK &= ~(1<<TOIE2);
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	TIMSK &= ~(1<<OCIE2);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2 |= ((1<<CS21) | (1<<CS20));
		TCCR2 &= ~(1<<CS22);
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 256
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 256.0;
	}
#endif

	tcnt2 = 256 - (int)((float)F_CPU * 0.0005 / prescaler);

    // Record whether we are in single or double buffer mode
    displayMode = mode;
    videoFlipPage = false;

    // Point the display buffer to the first physical buffer
    displayBuffer = &leds[0];
    displayPointer = displayBuffer->pixels;

    // If we are in single buffered mode, point the work buffer
    // at the same physical buffer as the display buffer.  Otherwise,
    // point it at the second physical buffer.
    if( displayMode & DOUBLE_BUFFER ) {
        workBuffer = &leds[1];
    }
    else {
        workBuffer = displayBuffer;
    }

    // Set up the timer buffering
    frontTimer = &timer[0];
    backTimer = &timer[1];

    videoFlipTimer = false;
    SetBrightness(127);
	
    // Clear the buffer and display it
    LedSign::Clear(0);
    LedSign::Flip(false);

    // Then start the display
	TCNT2 = tcnt2;
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TIMSK2 |= (1<<TOIE2);
#elif defined (__AVR_ATmega128__) || defined (__AVR_ATmega8__)
	TIMSK |= (1<<TOIE2);
#endif

    // If we are in double-buffer mode, wait until the display flips before we
    // return
    if (displayMode & DOUBLE_BUFFER)
    {
        while (videoFlipPage) {
            delay(1);
        }
    }

    initialized = true;
}


/* -----------------------------------------------------------------  */
/** Signal that the front and back buffers should be flipped
 * @param blocking if true : wait for flip before returning, if false :
 *                 return immediately.
 */
void LedSign::Flip(bool blocking)
{
    if (displayMode & DOUBLE_BUFFER)
    {
        // Just set the flip flag, the buffer will flip between redraws
        videoFlipPage = true;

        // If we are blocking, sit here until the page flips.
        while (blocking && videoFlipPage) {
            delay(1);
        }
    }
}


/* -----------------------------------------------------------------  */
/** Clear the screen completely
 * @param set if 1 : make all led ON, if not set or 0 : make all led OFF
 */
void LedSign::Clear(int set) {
    for(int x=0;x<14;x++)  
        for(int y=0;y<9;y++) 
            Set(x,y,set);
}


/* -----------------------------------------------------------------  */
/** Clear an horizontal line completely
 * @param y is the y coordinate of the line to clear/light [0-8]
 * @param set if 1 : make all led ON, if not set or 0 : make all led OFF
 */
void LedSign::Horizontal(int y, int set) {
    for(int x=0;x<14;x++)  
        Set(x,y,set);
}


/* -----------------------------------------------------------------  */
/** Clear a vertical line completely
 * @param x is the x coordinate of the line to clear/light [0-13]
 * @param set if 1 : make all led ON, if not set or 0 : make all led OFF
 */
void LedSign::Vertical(int x, int set) {
    for(int y=0;y<9;y++)  
        Set(x,y,set);
}


/* -----------------------------------------------------------------  */
/** Set : switch on and off the leds. All the position #for char in frameString:
 * calculations are done here, so we don't need to do in the
 * interrupt code
 */
void LedSign::Set(uint8_t x, uint8_t y, uint8_t c)
{
    // If we aren't in grayscale mode, just map any pin brightness to max
    if (c > 0 && !(displayMode & GRAYSCALE)) {
        c = SHADES-1;
    }

    uint8_t mask = ledMap[x+y*14].mask;
    uint8_t cycle = ledMap[x+y*14].cycle;

    uint8_t *p = &workBuffer->pixels[cycle*(SHADES-1)];
    int i;
    for (i = 0; i < c; i++)
	*p++ |= mask;   // ON;
    for (; i < SHADES-1; i++)
	*p++ &= ~mask;   // OFF;
}


/* Set the overall brightness of the screen
 * @param brightness LED brightness, from 0 (off) to 127 (full on)
 */

void LedSign::SetBrightness(uint8_t brightness)
{
    // An exponential fit seems to approximate a (perceived) linear scale
    float brightnessPercent = ((float)brightness / 127)*((float)brightness / 127);
    int difference = 0;

    /*   ---- This needs review! Please review. -- thilo  */
    // set up page counts
    // TODO: make SHADES a function parameter. This would require some refactoring.
    int start = 0;
    int max = 255;
    float scale = 1.5;
    float delta = pow( max - start , 1.0 / scale) / (SHADES - 1);
    uint8_t pageCounts[SHADES]; 

    pageCounts[0] = start;
    for (uint8_t i=1; i<SHADES; i++)
        pageCounts[i] = start + pow( i * delta, scale );

    if (! initialized) {
       // set front timer defaults
        for (int i = 0; i < SHADES; i++) {
            frontTimer->counts[i] = 255 - pageCounts[i];
            // TODO: Generate this dynamically
            frontTimer->prescaler[i] = slowPrescaler.TCCR2;
        }
    }

    // Wait until the previous brightness request goes through
    while( videoFlipTimer ) {
        delay(1);
    }

    // Compute on time for each of the pages
    // Use the fast timer; slow timer is only useful for < 3 shades.
    for (uint8_t i = 0; i < SHADES - 1; i++) {
        int interval = brightnessPercent
                       * (pageCounts[i + 1] - pageCounts[i])
                       * fastPrescaler.relativeSpeed;
        backTimer->counts[i] = 256 - (interval ? interval : 1);
        backTimer->prescaler[i] = fastPrescaler.TCCR2;
	difference += 256 - backTimer->counts[i];
    }

    // Compute off time
    backTimer->counts[SHADES - 1] = difference / fastPrescaler.relativeSpeed;
    backTimer->prescaler[SHADES - 1] = slowPrescaler.TCCR2;

    /*   ---- End of "This needs review! Please review." -- thilo  */

    // Have the ISR update the timer registers next run
    videoFlipTimer = true;
}


/* -----------------------------------------------------------------  */
/** The Interrupt code goes here !  
 */
ISR(TIMER2_OVF_vect) {
#ifdef MEASURE_ISR_TIME
    digitalWrite(statusPIN, HIGH);
#endif

    // For each cycle, we have potential SHADES pages to display.
    // Once every page has been displayed, then we move on to the next
    // cycle.

    // 24 Cycles of Matrix
    static uint8_t cycle = 0;

    // SHADES pages to display
    static uint8_t page = 0;

    TCCR2B = frontTimer->prescaler[page];
    TCNT2 = frontTimer->counts[page];

    if ( page == 0 ) {
        if (cycle == 0) {
            PORTB =            0;
        } else if (cycle == 12) {
            PORTD =            0;
        }
    } 

    if (page < SHADES - 1) {
        uint8_t x = *displayPointer++;
        if (cycle < 6) {
            DDRD  = _BV(cycle+2) | x;
            PORTD =            x;
            DDRB  =            0;
        } else if (cycle < 12) {
            DDRD =             x;
            PORTD =            x;
            DDRB  = _BV(cycle-6) | 0;
        } else if (cycle < 18) {
            DDRB  =            x;
            PORTB =            x;
            DDRD  = _BV(cycle+2-12) | 0;
        } else {
            DDRB  = _BV(cycle-6-12) | x;
            PORTB =            x;
            DDRD =             0;
        }
    } else {
        DDRD = 0;
        DDRB = 0;
    }

    page++;

    if (page >= SHADES) {
        page = 0;
        cycle++;

        if (cycle >= 24) {
            cycle = 0;

            // If the page should be flipped, do it here.
            if (videoFlipPage && (displayMode & DOUBLE_BUFFER))
            {
                // TODO: is this an atomic operation?
                videoFlipPage = false;
    
                videoPage* temp = displayBuffer;
                displayBuffer = workBuffer;
                workBuffer = temp;
            }

            if (videoFlipTimer) {
                videoFlipTimer = false;

                tempTimer = frontTimer;
                frontTimer = backTimer;
                backTimer = tempTimer;
            }

	    displayPointer = displayBuffer->pixels;
        }
    }

#ifdef MEASURE_ISR_TIME
    digitalWrite(statusPIN, LOW);
#endif
}
