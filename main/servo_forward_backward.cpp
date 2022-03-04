#include "header.h"

Servo myservo;
const uint8_t servoPin = 3;
int pos;
const unsigned long servo_rotate_interval = 15;
unsigned long servo_timer;


void servo_forward (void){

  Serial.println("I am in servo_forward");
  
  while (pos <= 180){
      Serial.println("I am inside first if stetement of servo_forward");

    if ( (millis () - servo_timer) > servo_rotate_interval){
      servo_timer = millis ();
      pos += 1;
      myservo.write(pos);
      Serial.println(pos);
    }
  }
}

void servo_backward (void){
  while (0 <= pos <= 180){
    if ( (millis () - servo_timer) > servo_rotate_interval){
      servo_timer = millis ();
      pos -= 1;
      myservo.write(pos);
      Serial.println(pos);
    }
  }
}
