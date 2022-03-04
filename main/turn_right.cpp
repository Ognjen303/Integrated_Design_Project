#include "header.h"


// could be later on usefull for debuging
// i_am_turning_right can be set to true only and the end of the else statement
// please do not set it as true anywhere else
bool i_am_turning_right = false;


// used to say inside the brackets: uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel istead of void

void turn_right(uint8_t right_velocity)
{

  //reset_all_flags();

  right_wheel_motor->setSpeed(100);
  left_wheel_motor->setSpeed(100);

  right_wheel_motor->run(FORWARD);
  left_wheel_motor->run(BACKWARD);

  /*if(i_am_turning_right)
    return;

    else
    {
    reset_all_flags();


    right_wheel_motor->setSpeed(100);
    left_wheel_motor->setSpeed(100);

    right_wheel_motor->run(FORWARD);
    left_wheel_motor->run(BACKWARD);

    i_am_turning_right = true;

    }*/
}

void turn_right_to_angle(float rotate_angle, uint8_t right_velocity) {
  unsigned long start_right_turn = millis(); // record time that the turning is started
  
  //Serial.println(1000*(rotate_angle*6.0/90.0));

  while (millis() - start_right_turn < int(1000 * (rotate_angle * 6.0 / 90.0))) { // using while loop to measure the time
    turn_right(right_velocity); // turning the robot left
  }
  
  //Serial.println("exiting left turn");
  stop_the_robot(); // stop turning
}
