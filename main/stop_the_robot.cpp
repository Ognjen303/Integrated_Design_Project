#include "header.h"

void stop_the_robot(void)
{
      // stopping the motors
      right_wheel_motor->run(RELEASE);
      left_wheel_motor->run(RELEASE);
}
