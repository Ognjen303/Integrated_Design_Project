#include "header.h"


// D8 for amber, D9 for red, D10 for green led
const byte amberLED = 8;
const byte redLED = 9;
const byte greenLED = 10;


// Time periods of blinks in milliseconds (1000 to a second).
const unsigned long amberLEDinterval = 500;
const unsigned long redLEDinterval = 200;
const unsigned long greenLEDinterval = 200;


// Variable holding the timer value so far. One for each "Timer"
unsigned long amberLEDtimer;
unsigned long redLEDtimer;
unsigned long greenLEDtimer;

/*
void setup() {
// setup the pinmode for leds
  pinMode(amberLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
// set timer value for blinky leds 
  amberLEDtimer = millis ();
  redLEDtimer = millis ();
  greenLEDtimer = millis ();
  
  Serial.begin(9600);
}

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
  if ( (millis () - amberLEDtimer) >= amberLEDinterval){
  // toggleAmberLED ();
  }

//main code for color detector
void DetectColour(void) {
  
  // reading analog value from arduino, A0 for distance sensor, A1 for colour detector 
  int distance_sensorValue = analogRead(A0);
  int colour_sensorValue = analogRead(A1);
  
  Serial.println(colour_sensorValue);

  //if analog reading larger than 1000, close enough, robot stopped, start to detect colour
  if (distance_sensorValue > 800){
    // stop the car for the colour detector to detect colour
    digitalWrite(amberLED, HIGH);
    
    // detecting colour
    if (colour_sensorValue > 250){
      // analog for red is about 300 and blue about 160, test after integration
      // red led on in this case
      if ( (millis () - redLEDtimer) >= redLEDinterval){
        toggleRedLED ();
      }
    }

    
    else if (100 < colour_sensorValue < 200){
      // green led on in this case
      if ( (millis () - greenLEDtimer) >= greenLEDinterval){
        toggleGreenLED ();
      }
    }

    
    /*else{
      // indicates colour detector does not work
      // any solution???
    
    } */

//  }
//}
// after detecting colour, wait for a few secs and toggle the servo/ turn off the leds?
