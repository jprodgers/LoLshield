/*
 Base 12 Clock
 
 Writen for the LoL Shield, designed by Jimmie Rodgers:
 http://jimmieprodgers.com/kits/lolshield/
 
 This needs the Charliplexing library, which you can get at the
 LoL Shield project page: http://code.google.com/p/lolshield/
 
 This also uses the Adafruit DS1307 breakout, which you will also
 need the library for:
 http://www.ladyada.net/learn/breakoutplus/ds1307rtc.html
 
 Created by Jimmie Rodgers on 2/2/2011.
 
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

//These libraries need to be included for both the LoL Shield
//and DS1307 breakout.
#include "Charliplexing.h"
#include "WProgram.h"
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;

void setup () {
  LedSign::Init(); //initializes the LoL Shield frame buffer
  pinMode(16, OUTPUT); //16 and 17 power the DS1307
  pinMode(17, OUTPUT);
  digitalWrite(16, LOW); //ground for the DS1307
  digitalWrite(17, HIGH);//provides 5v for the DS1307
  Wire.begin(); //starts the I2C serial on pins 18&19
  RTC.begin(); //starts communication with the DS1307
}

void loop(){
  DateTime now = RTC.now(); //creates a DateTime object
  //I set the time all at once so that it doesn't cause
  //strange timing issues
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();

  //These are used to easily parse the seconds
  int fiveCount = second / 5;
  int tenCount = second % 10;

  //It only needs to clear on counts of  10, as that
  //is the only time the display really changes.
  if (tenCount == 0 )LedSign::Clear();

  //These loops set the seconds on the LoL Shield.
  for (int i=0; i < tenCount+1; i++){
    if (i < 5)LedSign::Set(i+1, 8, 1);
    else LedSign::Set(i+3, 8, 1);
  }
  for (int i=0; i < fiveCount+1;i++){
    LedSign::Set(i+1, 7, 1);
  }

  //This loop sets the hour.
  for (int x=0; x < hour; x++){
    if(x < 12){
      if (x < 6)LedSign::Set(x, 0, 1);
      else LedSign::Set(x+1, 0, 1);
    }
    else{
      if (x < 18)LedSign::Set(x-12, 1, 1);
      else LedSign::Set(x-11, 1, 1);
    }
  }

  //This loop sets the minutes 
  for (int x=0; x < minute; x++){
    if(x < 12){
      if (x < 6)LedSign::Set(x+1, 2, 1);
      else LedSign::Set(x+2, 2, 1);
    }
    else if (x < 24){
      if (x < 18)LedSign::Set(x-11, 3, 1);
      else LedSign::Set(x-10, 3, 1);
    }
    else if (x < 36){
      if (x < 30)LedSign::Set(x-23, 4, 1);
      else LedSign::Set(x-22, 4, 1);
    }
    else if (x < 48){
      if (x < 42)LedSign::Set(x-35, 5, 1);
      else LedSign::Set(x-34, 5, 1);
    }
    else if (x < 60){
      if (x < 54)LedSign::Set(x-47, 6, 1);
      else LedSign::Set(x-46, 6, 1);
    }
  }
  delay(500);//no reason to update much more than this
}
