#include "header.h"

void go_backward(uint8_t velocity)
{
    // velocity has value between 0 and 255
    
    right_wheel_motor->setSpeed(velocity);
    left_wheel_motor->setSpeed(velocity);
    
    right_wheel_motor->run(BACKWARD);
    left_wheel_motor->run(BACKWARD);
}
