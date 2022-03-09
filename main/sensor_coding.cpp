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
  
  Serial.println("I am inside of flashamberled");
  Serial.println(amberLEDinterval);
  if ( (millis () - amberLEDtimer) >= amberLEDinterval){
    toggleAmberLED ();
  }
}

// stop/start the sensor system (stopped if servo already rotated)
void ToggleDetectingSystem(void) 
{
  if (pos == final_angle)
  { 
  ! sensor_status;
  }
}

// stop/start colour sensor 
void ToggleColourSensor(void) 
{
  uint16_t distance_sensorValue = analogRead(A0); // value between 0 and 1023
  Serial.println(distance_sensorValue);
  //if analog reading larger than 700, close enough, robot stopped, start to detect colour
  if (distance_sensorValue > 700 && sensor_status)
  {
   colour_detector;
  }
  else if (distance_sensorValue <= 700 && sensor_status)
  {
    unsigned long time_go_forward_distance = millis();
    while (millis() < time_go_forward_distance + 1000)
    {
      // robot goes forward for 1 second
      go_forward(100);
    } 
  }
}

//main code for color detector

void DetectColour(void) 
{
  // reading analog value from arduino, A0 for distance sensor, A1 for colour detector 
  uint16_t colour_sensorValue = analogRead(A1);   // value between 0 and 1023
  
  
  Serial.println("entering the function");


  //if analog reading larger than 800, close enough, robot stopped, start to detect colour
  if (colour_detector)
  {
    
    // stop the car for the colour detector to detect colour

    stop_the_robot();
    
    // detecting colour   

    unsigned long start_timer_and_count_5_seconds = millis();

    // loop for 3 seconds
    while(millis() < start_timer_and_count_5_seconds + 5000)
    {
      Serial.println(colour_sensorValue);
      // Red colour
      if (colour_sensorValue > 280)
      {
        
        digitalWrite (greenLED, LOW);
        // analog for red is about 300 and blue about 160, test after integration
        // red led on in this case
        digitalWrite (redLED, HIGH);
        
      }
  
      // Green colour
      else if (90 < colour_sensorValue < 200)
      {
        
        // green led on in this case
        digitalWrite (redLED, LOW);
        digitalWrite (greenLED, HIGH);
      }

    }
    
    unsigned long time_go_forward_colour = millis();
    while (millis() < time_go_forward_colour + 3000)
    {
      // robot goes forward for 1 second
      go_forward(100);
    }
    unsigned long time_stop_robot = millis();
    while (millis() < time_stop_robot + 1000)
    {
      // robot stops for 1 second
      stop_the_robot();
    }
    
    // rotate the servo to grab the block
    servo_forward();
    ! colour_detector;
    
  }
}
