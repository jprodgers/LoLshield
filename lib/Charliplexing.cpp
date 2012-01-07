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
#include <avr/interrupt.h>
#include "Charliplexing.h"

volatile unsigned int LedSign::tcnt2;


struct videoPage {
    uint8_t pixels[COLORS-1][48];  // TODO: is 48 right?
}; 

/* -----------------------------------------------------------------  */
/** Table for the LED multiplexing cycles
 * Each frame is made of 24 bytes (for the 24 display cycles)
 * There are 3 frames per buffer in grayscale mode (one for each brigtness)
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

/// Pointer to the buffer that should currently be drawn to
videoPage* workBuffer;

// For grayscale support
// TODO: map these based on tcnt2 calculation
//uint8_t pageCounts[COLORS] = {250, 205, 88, 0};
uint8_t pageCounts[COLORS] = {240, 205, 88, 0};


/// Flag indicating that the pageCounts buffer should be flipped as soon as the
/// current frame is displayed
volatile boolean videoFlipTimer = false;


// Timer counts to display each page for, plus off time
typedef struct timerInfo {
    uint8_t counts[COLORS];
    uint8_t prescaler[COLORS];
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

/// Uncomment to set analog pin 5 high during interrupts, so that an
/// oscilloscope can be used to measure the processor time taken by it
#define MEASURE_ISR_TIME
#ifdef MEASURE_ISR_TIME
uint8_t statusPIN = 19;
#endif

typedef struct LEDPosition {
    uint8_t high;
    uint8_t low;
};


/* -----------------------------------------------------------------  */
/** Table for LED Position in leds[] ram table
 */

const LEDPosition ledMap[126] = {
    {13, 5}, {13, 6}, {13, 7}, {13, 8}, {13, 9}, {13,10}, {13,11}, {13,12},
    {13, 4}, { 4,13}, {13, 3}, { 3,13}, {13, 2}, { 2,13},
    {12, 5}, {12, 6}, {12, 7}, {12, 8}, {12, 9}, {12,10}, {12,11}, {12,13},
    {12, 4}, { 4,12}, {12, 3}, { 3,12}, {12, 2}, { 2,12},
    {11, 5}, {11, 6}, {11, 7}, {11, 8}, {11, 9}, {11,10}, {11,12}, {11,13},
    {11, 4}, { 4,11}, {11, 3}, { 3,11}, {11, 2}, { 2,11},
    {10, 5}, {10, 6}, {10, 7}, {10, 8}, {10, 9}, {10,11}, {10,12}, {10,13},
    {10, 4}, { 4,10}, {10, 3}, { 3,10}, {10, 2}, { 2,10},
    { 9, 5}, { 9, 6}, { 9, 7}, { 9, 8}, { 9,10}, { 9,11}, { 9,12}, { 9,13},
    { 9, 4}, { 4, 9}, { 9, 3}, { 3, 9}, { 9, 2}, { 2, 9},
    { 8, 5}, { 8, 6}, { 8, 7}, { 8, 9}, { 8,10}, { 8,11}, { 8,12}, { 8,13},
    { 8, 4}, { 4, 8}, { 8, 3}, { 3, 8}, { 8, 2}, { 2, 8},
    { 7, 5}, { 7, 6}, { 7, 8}, { 7, 9}, { 7,10}, { 7,11}, { 7,12}, { 7,13},
    { 7, 4}, { 4, 7}, { 7, 3}, { 3, 7}, { 7, 2}, { 2, 7},
    { 6, 5}, { 6, 7}, { 6, 8}, { 6, 9}, { 6,10}, { 6,11}, { 6,12}, { 6,13},
    { 6, 4}, { 4, 6}, { 6, 3}, { 3, 6}, { 6, 2}, { 2, 6},
    { 5, 6}, { 5, 7}, { 5, 8}, { 5, 9}, { 5,10}, { 5,11}, { 5,12}, { 5,13},
    { 5, 4}, { 4, 5}, { 5, 3}, { 3, 5}, { 5, 2}, { 2, 5},
};


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

    // And write some default stuff into the front timer
    for (int i = 0; i < COLORS; i++) {
        frontTimer->counts[i] = pageCounts[i];
        // TODO: Generate this dynamically
        frontTimer->prescaler[i] = slowPrescaler.TCCR2;
    }

    LedSign::SetBrightness(127);
	
    // Clear the buffer and display it
    LedSign::Clear(0);
    LedSign::Flip(false);

    // Then start the display
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
	TCNT2 = tcnt2;
	TIMSK2 |= (1<<TOIE2);
#elif defined (__AVR_ATmega128__)
	TCNT2 = tcnt2;
	TIMSK |= (1<<TOIE2);
#elif defined (__AVR_ATmega8__)
	TCNT2 = tcnt2;
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
    uint8_t pin_high = ledMap[x+y*14].high;
    uint8_t pin_low  = ledMap[x+y*14].low;
    // pin_low is directly the address in the led array (minus 2 because the 
    // first two bytes are used for RS232 communication), but
    // as it is a two byte array we need to check pin_high also.
    // If pin_high is bigger than 8 address has to be increased by one

    uint8_t bufferNum = (pin_low-2)*2 + (pin_high / 8) + ((pin_high > 7)?24:0);
    uint8_t work = _BV(pin_high & 0x07);

    // If we aren't in grayscale mode, just map any pin brightness to max
    if (c > 0 && !(displayMode & GRAYSCALE)) {
        c = COLORS-1;
    }

    for (int i = 0; i < COLORS-1; i++) {
        if( c > i ) {
            workBuffer->pixels[i][bufferNum] |= work;   // ON
        }
        else {
            workBuffer->pixels[i][bufferNum] &= ~work;   // OFF
        }
    }
}


/* Set the overall brightness of the screen
 * @param brightness LED brightness, from 0 (off) to 127 (full on)
 */
void LedSign::SetBrightness(uint8_t brightness)
{
    Serial.print("starting ");

    // Wait until the previous brightness request goes through
    while( videoFlipTimer )
    {
        delay(1);
    }

    // An exponential fit seems to approximate a (perceived) linear scale
    float brightnessPercent = ((float)brightness / 127)*((float)brightness / 127);

    uint8_t difference = 0;

    Serial.print((int)brightness);
    Serial.print(" ");

    Serial.print((int)(brightnessPercent*100));
    Serial.print(" ");

    // Compute on time for each of the pages
    for (uint8_t i = 0; i < COLORS - 1; i++) {
        uint8_t interval = 255 - pageCounts[i];

        backTimer->counts[i] = 255 - brightnessPercent*interval;

        difference += backTimer->counts[i] - pageCounts[i];

        if (backTimer->counts[i] < 240) {
            // use low-pres prescaler
            backTimer->prescaler[i] = slowPrescaler.TCCR2;
        }
        else {
            // use high-res prescaler
            backTimer->counts[i] = 255 - brightnessPercent*interval*fastPrescaler.relativeSpeed;

            backTimer->prescaler[i] = fastPrescaler.TCCR2;
        }

        Serial.print((int)backTimer->counts[i]);
        Serial.print(":");
        Serial.print((int)backTimer->prescaler[i]);
        Serial.print(" ");
    }

    // Compute off time
    backTimer->counts[COLORS - 1] = 255 - difference;
    backTimer->prescaler[COLORS - 1] = slowPrescaler.TCCR2;

    Serial.print((int)backTimer->counts[COLORS - 1]);
    Serial.print(":");
    Serial.print((int)backTimer->prescaler[COLORS - 1]);
    Serial.print(" ");

    // Then update the registers
    videoFlipTimer = true;
    Serial.print("done\n");
}


/* -----------------------------------------------------------------  */
/** The Interrupt code goes here !  
 */
ISR(TIMER2_OVF_vect) {
        DDRD  = 0x0;
        DDRB  = 0x0;
#ifdef MEASURE_ISR_TIME
    digitalWrite(statusPIN, HIGH);
#endif

    // For each cycle, we have potential COLORS pages to display.
    // Once every page has been displayed, then we move on to the next
    // cycle.

    // 24 Cycles of Matrix
    static uint8_t cycle = 0;

    // COLORS pages to display
    static uint8_t page = 0;

#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || (__AVR_ATmega1280__)
    TCCR2B = frontTimer->prescaler[page];
    TCNT2 = frontTimer->counts[page];
#elif defined (__AVR_ATmega128__)
    TCCR2B = frontTimer->prescaler[page];
    TCNT2 = frontTimer->counts[page];
#elif defined (__AVR_ATmega8__)
    TCCR2B = frontTimer->prescaler[page];
    TCNT2 = frontTimer->counts[page];
#endif

    if ( page < COLORS - 1) { 

        if (cycle < 6) {
            DDRD  = _BV(cycle+2) | displayBuffer->pixels[page][cycle*2];
            PORTD =            displayBuffer->pixels[page][cycle*2];

            DDRB  =            displayBuffer->pixels[page][cycle*2+1];
            PORTB =            displayBuffer->pixels[page][cycle*2+1];
        } else if (cycle < 12) {
            DDRD =             displayBuffer->pixels[page][cycle*2];
            PORTD =            displayBuffer->pixels[page][cycle*2];

            DDRB  = _BV(cycle-6) | displayBuffer->pixels[page][cycle*2+1];
            PORTB =            displayBuffer->pixels[page][cycle*2+1];      
        } else if (cycle < 18) {
            DDRD  = _BV(cycle+2-12) | displayBuffer->pixels[page][cycle*2];
            PORTD =            displayBuffer->pixels[page][cycle*2];

            DDRB  =            displayBuffer->pixels[page][cycle*2+1];
            PORTB =            displayBuffer->pixels[page][cycle*2+1];
        } else {
            DDRD =             displayBuffer->pixels[page][cycle*2];
            PORTD =            displayBuffer->pixels[page][cycle*2];

            DDRB  = _BV(cycle-6-12) | displayBuffer->pixels[page][cycle*2+1];
            PORTB =            displayBuffer->pixels[page][cycle*2+1];      
        }
    } 
    else {
        // Turn everything off
        DDRD  = 0x0;
        DDRB  = 0x0;
    }

    page++;

    if (page >= COLORS) {
        page = 0;
        cycle++;
    }

    if (cycle > 24) {
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
    }

#ifdef MEASURE_ISR_TIME
    digitalWrite(statusPIN, LOW);
#endif
}
