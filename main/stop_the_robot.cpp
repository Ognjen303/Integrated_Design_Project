#include "header.h"


bool i_stopped = false; 


void stop_the_robot(void)
{
      right_wheel_motor->run(RELEASE);
      left_wheel_motor->run(RELEASE);
  
    /*if(i_stopped)
      return;

    else
    {
      reset_all_flags();
      
      right_wheel_motor->run(RELEASE);
      left_wheel_motor->run(RELEASE);
  
      i_stopped = true;
    }*/
}
