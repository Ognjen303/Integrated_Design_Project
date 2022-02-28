#include "header.h"

void turn_right(uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel)
{
    right_wheel_motor->setSpeed(velocity_of_right_wheel);
    left_wheel_motor->setSpeed(velocity_of_left_wheel);
    
    right_wheel_motor->run(FORWARD);
    left_wheel_motor->run(BACKWARD);  
}
