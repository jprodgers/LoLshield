
#include "Charliplexing.h"
#include "Myfont.h"

#include "Arduino.h"


  int leng=0; //provides the length of the char array
  unsigned char test[]="full ASCII charset: $ % & ! [ ] { } \0"; //text has to end with '\0' !!!!!!

/* -----------------------------------------------------------------  */
/** MAIN program Setup
 */
void setup()                    // run once, when the sketch starts
{
  LedSign::Init();
    for(int i=0; ; i++){ //get the length of the text
    if(test[i]==0){
      leng=i;
      break;
    }
    
  }
}


/* -----------------------------------------------------------------  */
/** MAIN program Loop
 */

void loop()                     // run over and over again
{

    
  
  Myfont::Banner(leng,test);


}
