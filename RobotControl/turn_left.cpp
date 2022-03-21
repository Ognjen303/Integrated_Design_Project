#include "header.h"

void turn_left(uint8_t left_velocity)
{
  
  right_wheel_motor->setSpeed(left_velocity);                   // velocity_of_right_wheel
  left_wheel_motor->setSpeed(left_velocity);                    // velocity_of_left_wheel

  right_wheel_motor->run(FORWARD);
  left_wheel_motor->run(BACKWARD);
}


void turn_left_to_angle(float rotate_angle, uint8_t left_velocity) {
  
  unsigned long start_left_turn = millis(); // record time that the turning is started
  while (millis() - start_left_turn < int(1000 * (rotate_angle * 6.0 / 360.0))) { // 6 secs to turn 360 degrees
    turn_left(left_velocity); // turning the robot left
    read_from_wifi(); // update parameters
    flashamberled();
  }
  stop_the_robot(); // stop turning
}
