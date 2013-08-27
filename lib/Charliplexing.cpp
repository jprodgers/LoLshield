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
#include <avr/pgmspace.h>
#include "Charliplexing.h"


struct videoPage {
    uint16_t pixels[12*(SHADES-1)];
}; 

/* -----------------------------------------------------------------  */
/** Table for the LED multiplexing cycles
 * Each frame is made of 24 bytes (for the 24 display cycles)
 * There are SHADES frames per buffer in grayscale mode (one for each brigtness)
 * and twice that many to support double-buffered grayscale.
 */
#ifdef DOUBLE_BUFFER
videoPage leds[2];
#else
videoPage leds[1];
#endif

/// Determines whether the display is in single or double buffer mode
uint8_t displayMode = SINGLE_BUFFER;

/// Flag indicating that the display page should be flipped as soon as the
/// current frame is displayed
#ifdef DOUBLE_BUFFER
volatile boolean videoFlipPage = false;
#endif

/// Pointer to the buffer that is currently being displayed
videoPage* displayBuffer;
uint16_t* displayPointer;

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

timerInfo timer[2];

// Record a slow and fast prescaler for later use
typedef struct prescalerInfo {
    uint8_t relativeSpeed;
    uint8_t tccr2;
};

prescalerInfo slowPrescaler, fastPrescaler;

static bool initialized = false;

/// Uncomment to set analog pin 5 high during interrupts, so that an
/// oscilloscope can be used to measure the processor time taken by it
#undef MEASURE_ISR_TIME
#ifdef MEASURE_ISR_TIME
uint8_t statusPIN = 19;
#endif

typedef struct LEDPosition {
    uint8_t high;
    uint8_t cycle;
};


/* -----------------------------------------------------------------  */
/** Table for LED Position in leds[] ram table
 */

#if defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__)
#define	P(pin)  ((pin < 5) ? (pin + 1) : (pin == 5) ? (2) : (pin))
#elif defined (__AVR_ATmega32U4__)
#define	P(pin)	((pin == 2) ? (1) : (pin == 3) ? (0) : (pin == 5) ? (2) : (pin == 6) ? (7) : (pin == 7) ? (5) : (pin == 12) ? (6) : (pin == 13) ? (3) : (pin))
#else
#define	P(pin)	(pin)
#endif
#if !defined (__AVR_ATmega32U4__)
#define L(high, low)	{ P(high), (P(low) - 2) }
#else
// Since the offset of 2 doesn't have to do anything with the ports anymore and just adds complexity we omit it.
#define L(high, low)	{ P(high), P(low) }
#endif
const LEDPosition PROGMEM ledMap[126] = {
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

/*
Converting the pin numbers to indices usable with Leonardo.

pin number -> Leonardo port number -> logical index
---------------------------------------------------
02         -> D1                   -> 01
03         -> D0                   -> 00
04         -> D4                   -> 04
05         -> C6                   -> 02
06         -> D7                   -> 07
07         -> E6                   -> 05
08         -> B4                   -> 08
09         -> B5                   -> 09
10         -> B6                   -> 10
11         -> B7                   -> 11
12         -> D6                   -> 06
13         -> C7                   -> 03

This yields the horrible macro
#define P(pin)  ((pin == 2) ? (1) : (pin == 3) ? (0) : (pin == 5) ? (2) : (pin == 6) ? (7) : (pin == 7) ? (5) : (pin == 12) ? (6) : (pin == 13) ? (3) : (pin))
TODO If anyone has a better idea how to handle this, feel free to change it.
TODO Another possibility would be to just add another ledMap without the remap macros.

The order in which the LEDs light up is now pretty random, but shouldn't be visible with a high update rate.

The used ports are
B7, B6, B5, B4, --, --, --, --
C7, C6, --, --, --, --, --, --
D7, D6, --, D4, --, --, D1, D0
--, E6, --, --, --, --, --, --

Luckily, these can merge together into the following contiguous order, stored in the 16bit pixels variable.
--, --, --, --, B7, B6, B5, B4
D7, D6, E6, D4, C7, C6, D1, D0

Now the ISR can work in the same way, it just has to extract the ports like this
PORTB = (pixels >> 4) & 0xF0);
PORTC = (pixels << 4) & 0xC0);
PORTD = (pixels << 0) & 0xD3);
PORTE = (pixels << 1) & 0x40);
*/

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

#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__)
    TIMSK2 &= ~(_BV(TOIE2) | _BV(OCIE2A));
    TCCR2A &= ~(_BV(WGM21) | _BV(WGM20));
    TCCR2B &= ~_BV(WGM22);
    ASSR &= ~_BV(AS2);
#elif defined (__AVR_ATmega8__)
    TIMSK &= ~(_BV(TOIE2) | _BV(OCIE2));
    TCCR2 &= ~(_BV(WGM21) | _BV(WGM20));
    ASSR &= ~_BV(AS2);
#elif defined (__AVR_ATmega128__)
    TIMSK &= ~(_BV(TOIE2) | _BV(OCIE2));
    TCCR2 &= ~(_BV(WGM21) | _BV(WGM20));
#elif defined (__AVR_ATmega32U4__)
    // The only 8bit timer on the Leonardo is used by default, so we use the 16bit Timer1
    // in CTC mode with a compare value of 256 to achieve the same behaviour.
    TIMSK1 &= ~(_BV(TOIE1) | _BV(OCIE1A));
    TCCR1A &= ~(_BV(WGM10) | _BV(WGM11));
    OCR1A = 256;
#endif
	
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega8__)
    if (F_CPU < 8000000UL) {
        fastPrescaler.tccr2 = _BV(CS20);		// 1
        slowPrescaler.tccr2 = _BV(CS21);		// 8
        fastPrescaler.relativeSpeed = 8;
    } else { // F_CPU >= 8Mhz, prescaler set to 8
        fastPrescaler.tccr2 = _BV(CS21);		// 8
        slowPrescaler.tccr2 = _BV(CS21) | _BV(CS20);	// 32
        fastPrescaler.relativeSpeed = 4;
    }
#elif defined (__AVR_ATmega128__)
    if (F_CPU < 8000000UL) {	// prescaler set to 8
        fastPrescaler.tccr2 = _BV(CS20);		// 1
        slowPrescaler.tccr2 = _BV(CS21);		// 8
        fastPrescaler.relativeSpeed = 8;
    } else { // F_CPU >= 8Mhz, prescaler set to 8
        fastPrescaler.tccr2 = _BV(CS21);		// 8
        slowPrescaler.tccr2 = _BV(CS21) | _BV(CS20);	// 64
        fastPrescaler.relativeSpeed = 8;
    }
#elif defined (__AVR_ATmega32U4__)
    if (F_CPU < 8000000UL) {	// prescaler set to 8
        fastPrescaler.tccr2 = _BV(WGM12) | _BV(CS10);                // 1
        slowPrescaler.tccr2 = _BV(WGM12) | _BV(CS11);                // 8
        fastPrescaler.relativeSpeed = 8;
    } else { // F_CPU >= 8Mhz, prescaler set to 8
        fastPrescaler.tccr2 = _BV(WGM12) | _BV(CS11);                // 8
        slowPrescaler.tccr2 = _BV(WGM12) | _BV(CS11) | _BV(CS10);    // 64
        fastPrescaler.relativeSpeed = 8;
    }
#endif
    slowPrescaler.relativeSpeed = 1;

    // Record whether we are in single or double buffer mode
    displayMode = mode;
#ifdef DOUBLE_BUFFER
    videoFlipPage = false;
#endif

    // Point the display buffer to the first physical buffer
    displayBuffer = &leds[0];
    displayPointer = displayBuffer->pixels;

#ifdef DOUBLE_BUFFER
    // If we are in single buffered mode, point the work buffer
    // at the same physical buffer as the display buffer.  Otherwise,
    // point it at the second physical buffer.
    if( displayMode & DOUBLE_BUFFER ) {
        workBuffer = &leds[1];
    } else {
        workBuffer = displayBuffer;
    }
#else
    workBuffer = displayBuffer;
#endif

    // Set up the timer buffering
    frontTimer = &timer[0];
    backTimer = &timer[1];

    videoFlipTimer = false;
    SetBrightness(127);
	
    // Clear the buffer and display it
    LedSign::Clear(0);
#ifdef DOUBLE_BUFFER
    LedSign::Flip(false);
#endif

    // Then start the display
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__)
    TIMSK2 |= _BV(TOIE2);
    TCCR2B = fastPrescaler.tccr2;
#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega128__)
    TIMSK |= _BV(TOIE2);
    TCCR2 = fastPrescaler.tccr2;
#elif defined (__AVR_ATmega32U4__)
    // Enable output compare match interrupt
    TIMSK1 |= _BV(OCIE1A);
    TCCR1B = fastPrescaler.tccr2;
#endif
    // interrupt ASAP
#if !defined (__AVR_ATmega32U4__)
    TCNT2 = 255;
#else
    TCNT1 = 255;
#endif

#ifdef DOUBLE_BUFFER
    // If we are in double-buffer mode, wait until the display flips before we
    // return
    if (displayMode & DOUBLE_BUFFER)
    {
        while (videoFlipPage) {
            delay(1);
        }
    }
#endif

    initialized = true;
}


#ifdef DOUBLE_BUFFER
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
#endif


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
#ifdef GRAYSCALE
    // If we aren't in grayscale mode, just map any pin brightness to max
    if (c > 0 && !(displayMode & GRAYSCALE))
        c = SHADES-1;
#else
    if (c)
        c = SHADES-1;
#endif

    uint16_t mask = 1 << pgm_read_byte_near(&ledMap[x+y*14].high);
    uint8_t cycle = pgm_read_byte_near(&ledMap[x+y*14].cycle);

    uint16_t *p = &workBuffer->pixels[cycle*(SHADES-1)];
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
    const float brightnessPercent = ((float)brightness / 127)*((float)brightness / 127);

    /*   ---- This needs review! Please review. -- thilo  */
    // set up page counts
    // TODO: make SHADES a function parameter. This would require some refactoring.
    uint8_t i;
    const int max = 255;
#define	C(x)	(0.49999f + max * brightnessPercent * fastPrescaler.relativeSpeed * x)
#if SHADES == 2
    const int counts[SHADES] = {
	0.0f,
	C(1.0f),
    };
#elif SHADES == 8
    const int counts[SHADES] = {
	0.0f,
	C(0.030117819624378613658712f),
	C(0.104876339357015456218728f),
	C(0.217591430058779512857041f),
	C(0.365200625214741116475101f),
	C(0.545719579451565749226202f),
	C(0.757697368024318811680598f),
	C(1.0f),
    };
#else
    // NOTE: Changing "scale" invalidates any tables above!
    const float scale = 1.8f;
    int counts[SHADES]; 

    counts[0] = 0.0f;
    for (i=1; i<SHADES; i++)
        counts[i] = C(pow(i / (float)(SHADES - 1), scale));
#endif

    // Wait until the previous brightness request goes through
    while( videoFlipTimer ) {
        delay(1);
    }

    // Compute on time for each of the pages
    // Use the fast timer; slow timer is only useful for < 3 shades.
    for (i = 0; i < SHADES - 1; i++) {
        int interval = counts[i + 1] - counts[i];
        backTimer->counts[i] = 256 - (interval ? interval : 1);
        backTimer->prescaler[i] = fastPrescaler.tccr2;
    }

    // Compute off time
    int interval = max - counts[i] / fastPrescaler.relativeSpeed;
    backTimer->counts[i] = 256 - (interval ? interval : 1);
    backTimer->prescaler[i] = slowPrescaler.tccr2;

    if (!initialized)
        *frontTimer = *backTimer;

    /*   ---- End of "This needs review! Please review." -- thilo  */

    // Have the ISR update the timer registers next run
    videoFlipTimer = true;
}


/* -----------------------------------------------------------------  */
/** The Interrupt code goes here !  
 */
#if !defined (__AVR_ATmega32U4__)
ISR(TIMER2_OVF_vect) {
#else
ISR(TIMER1_COMPA_vect) {
#endif
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

#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__)
    TCCR2B = frontTimer->prescaler[page];
#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega128__)
    TCCR2 = frontTimer->prescaler[page];
#elif defined (__AVR_ATmega32U4__)
    TCCR1B = frontTimer->prescaler[page];
#endif
#if !defined (__AVR_ATmega32U4__)
    TCNT2 = frontTimer->counts[page];
#else
    TCNT1 = frontTimer->counts[page];
#endif

#if defined (__AVR_ATmega1280__) || defined (__AVR_ATmega2560__)
    // Turn everything off
    DDRE &= ~0x38;
    DDRG &= ~0x20;
    DDRH &= ~0x78;
    DDRB &= ~0xf0;

    // Pins on the Arduino MEGA 1280/2560 are mapped to different ports on the microcontroller
    // than other board types. Need to remap the bits in displayBuffer to the correct pins.
    // In this case, used ports have un-used pins. To allow these pins to be used,
    // their state is preserved when writing to the ports.
    if (page < SHADES - 1) {
        const uint16_t data = *displayPointer++, dir = data | (1 << (cycle+2));
        PORTE = (PORTE & (~0x38)) | ((data << 1) & 0x38);
        PORTG = (PORTG & (~0x20)) | ((data << 0) & 0x20);
        PORTH = (PORTH & (~0x78)) | ((data >> 3) & 0x78);
        PORTB = (PORTB & (~0xf0)) | ((data >> 6) & 0xf0);
        DDRE |= ((dir << 1) & 0x38);
        DDRG |= ((dir << 0) & 0x20);
        DDRH |= ((dir >> 3) & 0x78);
        DDRB |= ((dir >> 6) & 0xf0);
    }
#elif defined (__AVR_ATmega32U4__)
    // Turn everything off
    DDRB &= ~0xF0;
    DDRC &= ~0xC0;
    DDRD &= ~0xD3;
    DDRE &= ~0x40;

    if (page < SHADES - 1) {
        const uint16_t data = *displayPointer++, dir = data | (1 << (cycle));
        PORTB = (PORTB & (~0xF0)) | ((data >> 4) & 0xF0);
        PORTC = (PORTC & (~0xC0)) | ((data << 4) & 0xC0);
        PORTD = (PORTD & (~0xD3)) | ((data << 0) & 0xD3);
        PORTE = (PORTE & (~0x40)) | ((data << 1) & 0x40);
        DDRB |= ((dir >> 4) & 0xF0);
        DDRC |= ((dir << 4) & 0xC0);
        DDRD |= ((dir << 0) & 0xD3);
        DDRE |= ((dir << 1) & 0x40);
    }
#else
    // Turn everything off
    DDRD = 0;
    DDRB = 0;

    if (page < SHADES - 1) {
        const uint16_t data = *displayPointer++, dir = data | (1 << (cycle+2));
        DDRD  = dir;
        PORTD = data;
        DDRB  = (dir >> 8);
	PORTB = (data >> 8);
    }
#endif

    page++;

    if (page >= SHADES) {
        page = 0;
        cycle++;

        if (cycle >= 12) {
            cycle = 0;

#ifdef DOUBLE_BUFFER
            // If the page should be flipped, do it here.
            if (videoFlipPage && (displayMode & DOUBLE_BUFFER))
            {
                // TODO: is this an atomic operation?
                videoFlipPage = false;
    
                videoPage* temp = displayBuffer;
                displayBuffer = workBuffer;
                workBuffer = temp;
            }
#endif

            if (videoFlipTimer) {
                videoFlipTimer = false;

		timerInfo* temp = frontTimer;
                frontTimer = backTimer;
                backTimer = temp;
            }

	    displayPointer = displayBuffer->pixels;
        }
    }

#ifdef MEASURE_ISR_TIME
    digitalWrite(statusPIN, LOW);
#endif
}
