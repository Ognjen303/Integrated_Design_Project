#include "header.h"

void test_go_forward_and_back(uint8_t velocity)
{
    // Robot goes forward 5 sec and then back to starting position
    
    go_forward(velocity);
    
    delay(5000);
    Serial.println("I finished going forward.");
    
    // right_wheel_motor->run(RELEASE);
    // left_wheel_motor->run(RELEASE);
    
    go_backward(velocity);
    delay(5000);
    Serial.println("I finished going backwards.");
    
    right_wheel_motor->run(RELEASE);
    left_wheel_motor->run(RELEASE);
}
