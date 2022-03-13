#include "header.h"


// used to say inside the brackets: uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel istead of void

void turn_right(uint8_t right_velocity)
{

  //reset_all_flags();

  right_wheel_motor->setSpeed(right_velocity);
  left_wheel_motor->setSpeed(right_velocity);

  right_wheel_motor->run(BACKWARD);
  left_wheel_motor->run(FORWARD);

}


void turn_right_to_angle(float rotate_angle, uint8_t right_velocity) {
  unsigned long start_right_turn = millis(); // record time that the turning is started
  
  //Serial.println(1000*(rotate_angle*6.0/90.0));

  while (millis() - start_right_turn < int(1000 * (rotate_angle * 10 / 90.0))) { // using while loop to measure the time
    turn_right(right_velocity); // turning the robot left
    read_from_wifi();
    flashamberled();
  }
  
  //Serial.println("exiting left turn");
  stop_the_robot(); // stop turning
}
