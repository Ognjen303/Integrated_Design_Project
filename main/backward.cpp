#include "header.h"


// could be later on usefull for debuging
// i_am_going_backward can be set to true only and the end of the else statement
// please do not set it as true anywhere else
bool i_am_going_backward = false;


void go_backward(uint8_t velocity)
{
    //reset_all_flags();
      right_wheel_motor->setSpeed(velocity);
      left_wheel_motor->setSpeed(velocity);
      
      right_wheel_motor->run(BACKWARD);
      left_wheel_motor->run(BACKWARD);
    // velocity has value between 0 and 255

    /*
    if (i_am_going_backward)
        return;

    else
    {
      reset_all_flags();
      
      Serial.println("I started going backward, velocity is:");
      Serial.println(velocity);
      
      right_wheel_motor->setSpeed(velocity);
      left_wheel_motor->setSpeed(velocity);
      
      right_wheel_motor->run(BACKWARD);
      left_wheel_motor->run(BACKWARD);

      i_am_going_backward = true;

    }*/
}
