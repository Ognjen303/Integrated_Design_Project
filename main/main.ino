#include "header.h"


uint8_t velocity;
uint8_t velocity_of_right_wheel;
uint8_t velocity_of_left_wheel;

void setup()
{
    AFMS.begin();
    
    velocity = 150;
    velocity_of_right_wheel = 150;
    velocity_of_left_wheel = 150;
    
}

void loop()
{  

    turn_left(velocity_of_right_wheel, velocity_of_left_wheel);
    delay(15000);
    right_wheel_motor->run(RELEASE);
    left_wheel_motor->run(RELEASE);
    while(1);
    
    /*go_forward(velocity);
    delay(4000);
    go_backward(velocity);
    delay(4000);
    right_wheel_motor->run(RELEASE);
    left_wheel_motor->run(RELEASE);
    while(1);
    */
}
