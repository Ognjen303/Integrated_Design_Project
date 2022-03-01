#include "header.h"

void test_drive_in_a_square(uint8_t velocity)
{
    // robot goes froward for 5 seconds
    go_forward(velocity);
    delay(5000);
    

    // need to figure out how to make the robot turn by 90 degrees
    // turn 90 degrees
    turn_right(100, 100);
    delay(5500);

    

    go_forward(velocity);
    delay(5000);


    turn_right(100, 100);
    delay(5500);
    

    go_forward(velocity);
    delay(5000);


    turn_right(100, 100);
    delay(5500);
    
    
    go_forward(velocity);
    delay(5000);


    turn_right(100, 100);
    delay(5500);
    

    right_wheel_motor->run(RELEASE);
    left_wheel_motor->run(RELEASE);
}
