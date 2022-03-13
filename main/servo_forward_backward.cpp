#include "header.h"

Servo myservo;
const uint8_t servoPin = 9;
int pos;
const unsigned long servo_forward_interval = 15;
const unsigned long servo_backward_interval = 16;
unsigned long servo_timer;
const uint16_t final_angle = 165;


void servo_forward (void){
  Serial.println(myservo.read());
  for (pos = 0; pos <= final_angle; pos += 1)
  { // goes from 0 degrees to 90 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    servo_timer = millis();
    while( (millis () - servo_timer) < 15){
      flashamberled();
    }
  }
  Serial.println(myservo.read() );
  if (abs(myservo.read() - final_angle) > 2){
    Serial.println("servo blocked, no pun intended.");
  }
}

void servo_backward (void){
  Serial.println(myservo.read());
  for (pos = final_angle; pos >= 0; pos -= 1)
  { // goes from 0 degrees to 90 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    servo_timer = millis();
    while( (millis () - servo_timer) < 15){
      flashamberled();
    }
  }
}
