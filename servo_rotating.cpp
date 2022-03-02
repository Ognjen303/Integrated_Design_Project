#include "header.h"



Servo myservo;
const uint8_t servoPin = 10;

int pos = 0;    // variable to store the servo position


void servo_rotating(void)
{
  for (pos = 0; pos <= 90; pos += 1)
  { // goes from 0 degrees to 90 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  
}
