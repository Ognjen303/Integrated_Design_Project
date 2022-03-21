#include "header.h"

// initially set the sensor working and colour detector not working
bool sensor_status = true;
bool colour_detector = false;


// Time periods of blinks in milliseconds (1000 to a second).
const unsigned long amberLEDinterval = 500;


// Variable holding the timer value so far. One for each "Timer"
unsigned long amberLEDtimer;


// code for toggle leds/ flashing leds
void toggleAmberLED (void)
  {
   if (digitalRead (amberLED) == LOW)
      digitalWrite (amberLED, HIGH);
   else
      digitalWrite (amberLED, LOW);

  // remember when we toggled it
  amberLEDtimer = millis ();  
  }  // end of toggleRedLED



// flashing amber light in 2HZ
void flashamberled(void){
  
  // checking if amber LED needs to change state
  if ( (millis () - amberLEDtimer) >= amberLEDinterval){
    // toggle LED if true
    toggleAmberLED ();
  }
}
