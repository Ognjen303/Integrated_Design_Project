#include "header.h"

Servo myservo;
const uint8_t servoPin = 9;
int pos;
const unsigned long servo_forward_interval = 15;
const unsigned long servo_backward_interval = 16;
unsigned long servo_timer;


void servo_forward (void)
{

  Serial.println("I am in servo_forward");
  
  while (pos <= 110)
  {
    //Serial.println("I am inside first if stetement of servo_forward");

    if ( (millis () - servo_timer) > servo_forward_interval){
      servo_timer = millis ();
      pos += 1;
      myservo.write(pos);
      //Serial.println(pos);
    }
  }
}

void servo_backward (void){
  while (0 <= pos <= 111){
    if ( (millis () - servo_timer) > servo_backward_interval){
      servo_timer = millis ();
      pos -= 1;
      myservo.write(pos);
      Serial.println(pos);
    }
  }
}
