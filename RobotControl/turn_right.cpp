#include "header.h"

void turn_right(uint8_t right_velocity)
{
  // setting turn velocity
  right_wheel_motor->setSpeed(right_velocity);
  left_wheel_motor->setSpeed(right_velocity);

  right_wheel_motor->run(BACKWARD);
  left_wheel_motor->run(FORWARD);

}


void turn_right_to_angle(float rotate_angle, uint8_t right_velocity) {
  
  unsigned long start_right_turn = millis(); // record time that the turning is started
  while (millis() - start_right_turn < int(1000 * (rotate_angle * 6.0 / 360.0))) { // 6 secs for 360 degrees
    turn_right(right_velocity); // turning the robot left
    read_from_wifi(); // update parameters while turning
    flashamberled();
  }
  stop_the_robot(); // stop turning
}
