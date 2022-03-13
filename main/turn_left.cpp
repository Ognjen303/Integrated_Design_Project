#include "header.h"


// used to say inside the brackets: uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel istead of void

void turn_left(uint8_t left_velocity)
{
  // maybe try fixing the turning velocity of both wheels to 100
  
  //reset_all_flags();
  
  right_wheel_motor->setSpeed(left_velocity);                   // velocity_of_right_wheel
  left_wheel_motor->setSpeed(left_velocity);                    // velocity_of_left_wheel

  right_wheel_motor->run(FORWARD);
  left_wheel_motor->run(BACKWARD);
}


void turn_left_to_angle(float rotate_angle, uint8_t left_velocity) {
  unsigned long start_left_turn = millis(); // record time that the turning is started
  
  //Serial.println(1000*(rotate_angle*6.0/90.0));

  while (millis() - start_left_turn < int(1000 * (rotate_angle * 10.0 / 90.0))) { // using while loop to measure the time
    turn_left(left_velocity); // turning the robot left
    read_from_wifi();
    flashamberled();
  }
  
  //Serial.println("exiting left turn");
  stop_the_robot(); // stop turning
}
