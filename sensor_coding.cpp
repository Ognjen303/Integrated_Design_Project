#include "header.h"

// set to true if robot is detecting a colour of the block
bool i_am_detecting_colour = false; // flag is true if I am still trying to detect colour
bool i_am_detecting_red_colour = false;
bool i_am_detecting_blue_colour = false;


// Time periods of blinks in milliseconds (1000 to a second).
const unsigned long amberLEDinterval = 500;
const unsigned long redLEDinterval = 200;
const unsigned long greenLEDinterval = 200;


// Variable holding the timer value so far. One for each "Timer"
unsigned long amberLEDtimer;
unsigned long redLEDtimer;
unsigned long greenLEDtimer;



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


  
void toggleRedLED (void)
  {
   if (digitalRead (redLED) == LOW)
      digitalWrite (redLED, HIGH);
   else
      digitalWrite (redLED, LOW);

  // remember when we toggled it
  redLEDtimer = millis ();  
  }  // end of toggleRedLED

 
void toggleGreenLED (void)
  {
   if (digitalRead (greenLED) == LOW)
      digitalWrite (greenLED, HIGH);
   else
      digitalWrite (greenLED, LOW);

  // remember when we toggled it
  greenLEDtimer = millis ();  
  }  // end of toggleGreenLED




// flashing amber light in 2HZ
void flashamberled(void){
  
  Serial.println("I am inside of flashamberled");
  Serial.println(amberLEDinterval);
  if ( (millis () - amberLEDtimer) >= amberLEDinterval){
    toggleAmberLED ();
  }
}





//main code for color detector

void DetectColour(void) 
{
  // reading analog value from arduino, A0 for distance sensor, A1 for colour detector 
  
  uint16_t distance_sensorValue = analogRead(A0); // value between 0 and 1023
  uint16_t colour_sensorValue = analogRead(A1);   // value between 0 and 1023
  
  Serial.println("entering the function");

  Serial.println(distance_sensorValue);
  Serial.println(colour_sensorValue);

  //if analog reading larger than 800, close enough, robot stopped, start to detect colour
  if (distance_sensorValue > 800)
  {
    
    // stop the car for the colour detector to detect colour
    right_wheel_motor->run(RELEASE);
    left_wheel_motor->run(RELEASE);

    reset_all_flags();
    
    // detecting colour
    i_am_detecting_colour = true;  
    

    // Red colour
    if (colour_sensorValue > 250)
    {
      reset_all_flags();
      i_am_detecting_red_colour = true;
      
      
      digitalWrite (greenLED, LOW);
      // analog for red is about 300 and blue about 160, test after integration
      // red led on in this case
      if ( (millis () - redLEDtimer) >= redLEDinterval){
        toggleRedLED ();
      }
      
    }

    // Green colour
    else if (100 < colour_sensorValue < 200)
    {
      reset_all_flags();
      i_am_detecting_blue_colour = true;

      
      // green led on in this case
      digitalWrite (redLED, LOW);
      if ( (millis () - greenLEDtimer) >= greenLEDinterval){
        toggleGreenLED ();
      }
    }

    
    /*
     else{
      // indicates colour detector does not work
      // any solution???
    
    } */

  }
}
// after detecting colour, wait for a few secs and toggle the servo/ turn off the leds?
