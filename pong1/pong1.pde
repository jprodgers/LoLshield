
//**************************************************************//
//  Name    : Pong for Arduino / Charlieplexing                 //
//  Author  : Benjamin Sonntag http://benjamin.sonntag.fr/      //
//  Date    : 28 dec 2009                                       //
//  Version : 0.1                                               //
//  Notes   : Uses Charlieplexing techniques to light up        //
//          : a matrix of 126 LEDs in a 9x14 grid               //
//          : from Jimmie P Rodgers www.jimmieprodgers.com      //
//**************************************************************//

#include <avr/pgmspace.h>  //This is in the Arduino library 

int blinkdelay = 70; //This basically controls brightness. Lower is dimmer
int runspeed = 14;   //smaller = faster

int pin13 =13;
int pin12 =12;
int pin11 =11;
int pin10 =10;
int pin09 =9;
int pin08 =8;
int pin07 =7;
int pin06 =6;
int pin05 =5;
int pin04 =4;
int pin03 =3;
int pin02 =2;

const int pins[] = {
  pin13,pin12,pin11,pin10,pin09,pin08,pin07,pin06,pin05,pin04,pin03,pin02};

const int ledMap[126][2] ={
{pin13, pin05},{pin13, pin06},{pin13, pin07},{pin13, pin08},{pin13, pin09},{pin13, pin10},{pin13, pin11},{pin13, pin12},{pin13, pin04},{pin04, pin13},{pin13, pin03},{pin03, pin13},{pin13, pin02},{pin02, pin13},
{pin12, pin05},{pin12, pin06},{pin12, pin07},{pin12, pin08},{pin12, pin09},{pin12, pin10},{pin12, pin11},{pin12, pin13},{pin12, pin04},{pin04, pin12},{pin12, pin03},{pin03, pin12},{pin12, pin02},{pin02, pin12},
{pin11, pin05},{pin11, pin06},{pin11, pin07},{pin11, pin08},{pin11, pin09},{pin11, pin10},{pin11, pin12},{pin11, pin13},{pin11, pin04},{pin04, pin11},{pin11, pin03},{pin03, pin11},{pin11, pin02},{pin02, pin11},
{pin10, pin05},{pin10, pin06},{pin10, pin07},{pin10, pin08},{pin10, pin09},{pin10, pin11},{pin10, pin12},{pin10, pin13},{pin10, pin04},{pin04, pin10},{pin10, pin03},{pin03, pin10},{pin10, pin02},{pin02, pin10},
{pin09, pin05},{pin09, pin06},{pin09, pin07},{pin09, pin08},{pin09, pin10},{pin09, pin11},{pin09, pin12},{pin09, pin13},{pin09, pin04},{pin04, pin09},{pin09, pin03},{pin03, pin09},{pin09, pin02},{pin02, pin09},
{pin08, pin05},{pin08, pin06},{pin08, pin07},{pin08, pin09},{pin08, pin10},{pin08, pin11},{pin08, pin12},{pin08, pin13},{pin08, pin04},{pin04, pin08},{pin08, pin03},{pin03, pin08},{pin08, pin02},{pin02, pin08},
{pin07, pin05},{pin07, pin06},{pin07, pin08},{pin07, pin09},{pin07, pin10},{pin07, pin11},{pin07, pin12},{pin07, pin13},{pin07, pin04},{pin04, pin07},{pin07, pin03},{pin03, pin07},{pin07, pin02},{pin02, pin07},
{pin06, pin05},{pin06, pin07},{pin06, pin08},{pin06, pin09},{pin06, pin10},{pin06, pin11},{pin06, pin12},{pin06, pin13},{pin06, pin04},{pin04, pin06},{pin06, pin03},{pin03, pin06},{pin06, pin02},{pin02, pin06},
{pin05, pin06},{pin05, pin07},{pin05, pin08},{pin05, pin09},{pin05, pin10},{pin05, pin11},{pin05, pin12},{pin05, pin13},{pin05, pin04},{pin04, pin05},{pin05, pin03},{pin03, pin05},{pin05, pin02},{pin02, pin05}
};


uint16_t screens[][9] = {
  {0,240,264,516,516,516,516,264,240},
  {6148,4098,12387,8337,8457,8457,8337,12387,4098},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0}
};


// The figures from 0 to 9 encoded in 7 lines of 5 bits :
uint16_t figures[][7] = { 
{14,17,17,17,17,17,14},
{4,6,4,4,4,4,14},
{14,17,16,14,1,1,31},
{14,17,16,14,16,17,14},
{8,12,10,9,31,8,8},
{31,1,1,15,16,16,15},
{14,17,1,15,17,17,14},
{31,16,8,8,4,4,4},
{14,17,17,14,17,17,14},
{14,17,17,30,16,16,15},
};

  
int x,y,dx,dy;
int sh1y,sh2y,s1,s2;
int activescreen;

void setup() {
  x = 3;
  y = 7;
  sh1y=3;
  sh2y=3;
  dx = 1;
  dy = 1;
  s1 = 0;
  s2 = 0;
  activescreen = 0;
  randomSeed(analogRead(0));
}

void loop() {
  int ct1,ct2;


  // The Ball shall bounce on the walls : 
  if (x==12 || x==1) {
    dx=-dx;
    if (random(0,6)>4) {
      if (random(0,2)==1) {
          s1++; if (s1==10) s1=0;
      } else {
          s2++; if (s2==10) s2=0;
      }
      drawscores(s1,s2);
    }
  }
  if (y==8 || y==0) dy=-dy;
 
  
  // Clear the non-active screen
  for(ct2=0;ct2<9;ct2++) { 
    screens[1-activescreen][ct2]=0;
  }
  // Move the BALL : 
  x=x+dx; 
  y=y+dy;
  
  // Draw the ball :
  screens[1-activescreen][y]=1<<x;
  // Draw the Ship
  screens[1-activescreen][sh1y]+=1<<0;
  screens[1-activescreen][sh1y+1]+=1<<0;
  screens[1-activescreen][sh2y]+=1<<13;
  screens[1-activescreen][sh2y+1]+=1<<13;

  // The ships moves when the ball go in their direction. They follow it magically ;) :   
  if (dx<0) {
    if (sh1y>y) {
      sh1y--;
    }
    if (sh1y<y) {
      sh1y++;
    }
  } else {
    if (sh2y>y) {
      sh2y--;
    }
    if (sh2y<y) {
      sh2y++;
    }
  }
  if (sh1y>7) sh1y=7;
  if (sh2y>7) sh2y=7;
  if (sh1y<0) sh1y=0;
  if (sh2y<0) sh2y=0;

  // swap the screens ;) (sometime we may need this double-buffer algorithm...
  // of course, as of today it's a little bit overkill ...)
  activescreen=1-activescreen; 
  for(ct1=0;ct1<20;ct1++) {
    DisplayBitMap();
  }  
  // loop :) 
}


// Draw the scores in a lovely scrolling :) 
// Use the current active screen brutally ...
void drawscores(int s1,int s2) {
  int i,j,ps1,ps2,ct2;
  
  for(ps1=0;ps1<8;ps1++) {      
    // Clear the active screen
    for(ct2=0;ct2<9;ct2++) { 
      screens[activescreen][ct2]=0;
    }
    screens[activescreen][4]+=((1<<6) + (1<<7)); // -- in the middle
    // Fill it with both scores : 
    // Left score goes up>down
    ps2=8-ps1;
    i=ps1; j=6;
    while (i>=0 && j>=0) {
      screens[activescreen][i]+=figures[s1][j];
      i--; j--;
    }
    // Right score goes down>up
    i=ps2; j=0;
    while (i<=8 && j<=6) {
      screens[activescreen][i]+=figures[s2][j]<<9;
      i++; j++;
    }
    // Draw it for a little time ...
    for(ct2=0;ct2<20;ct2++) {
      DisplayBitMap();
    }
  }  
  for(ct2=0;ct2<150;ct2++) {
    DisplayBitMap();
  }
}



void turnon(int led) {
  int pospin = ledMap[led][0];
  int negpin = ledMap[led][1];

  pinMode (pospin, OUTPUT);
  pinMode (negpin, OUTPUT);
  digitalWrite (pospin, HIGH);
  digitalWrite (negpin, LOW);
}

void alloff() {
  DDRD = B00000010;
  DDRB = B00000000; 
}

void DisplayBitMap()
{
  //  boolean run=true;
  //  byte frame = 0;
  byte line = 0;
  unsigned long data;
  for(line = 0; line < 9; line++)       {
        data = screens[activescreen][line];
        for (byte led=0; led<14; ++led) {
          if (data & (1<<led)) {
            turnon((line*14)+led);
            delayMicroseconds(blinkdelay);
            alloff();
          }
          else {
            delayMicroseconds(blinkdelay);
          }
        }
      }
}


void blinkall(int numblink) {
  alloff();
  for(int n = 0;n < numblink;n++)   {
    for(int i = 0; i < runspeed; i++)     {
      for(int j = 0; j < 126; j++)       {
        turnon(j);
        delayMicroseconds(blinkdelay);
        alloff();
      }
    }
    delay(500);
  }
}

void sequenceon() {
  for(int i = 0; i < 126; i++)   {
    turnon(i);
    delay(800);
    alloff();
    delay(800);
  }
}


