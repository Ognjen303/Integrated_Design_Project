#include "header.h"


// could be later on usefull for debuging
// i_am_turning_right can be set to true only and the end of the else statement
// please do not set it as true anywhere else
bool i_am_turning_right = false;


// used to say inside the brackets: uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel istead of void

void turn_right(void)
{
    if(i_am_turning_right)
      return;

    else
    {
      reset_all_flags();

      
      right_wheel_motor->setSpeed(100);
      left_wheel_motor->setSpeed(100);
      
      right_wheel_motor->run(FORWARD);
      left_wheel_motor->run(BACKWARD);

      i_am_turning_right = true;

    }
}
