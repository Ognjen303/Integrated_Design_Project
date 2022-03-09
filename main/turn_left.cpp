#include "header.h"


// could be later on usefull for debuging
// i_am_turning_left can be set to true only and the end of the else statement
// please do not set it as true anywhere else
bool i_am_turning_left = false;



// used to say inside the brackets: uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel istead of void

void turn_left(uint8_t left_velocity)
{
  // maybe try fixing the turning velocity of both wheels to 100
  
  //reset_all_flags();
  
  right_wheel_motor->setSpeed(left_velocity);                   // velocity_of_right_wheel
  left_wheel_motor->setSpeed(left_velocity);                    // velocity_of_left_wheel

  right_wheel_motor->run(BACKWARD);
  left_wheel_motor->run(FORWARD);

  /*if(i_am_turning_left)
    return;

    else
    {
    reset_all_flags();

    right_wheel_motor->setSpeed(100);                   // velocity_of_right_wheel
    left_wheel_motor->setSpeed(100);                    // velocity_of_left_wheel

    right_wheel_motor->run(BACKWARD);
    left_wheel_motor->run(FORWARD);

    i_am_turning_left = true;

    }*/
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
