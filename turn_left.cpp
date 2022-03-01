#include "header.h"

void turn_left(uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel)
{
    // maybe try fixing the turning velocity of both wheels to 100

    
    right_wheel_motor->setSpeed(velocity_of_right_wheel);
    left_wheel_motor->setSpeed(velocity_of_left_wheel);
    
    right_wheel_motor->run(BACKWARD);
    left_wheel_motor->run(FORWARD);

    delay(5000);
    
    right_wheel_motor->run(RELEASE);
    left_wheel_motor->run(RELEASE);
    

}
