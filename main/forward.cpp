#include "header.h"


Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// rename later muMotor_1 and _2 to leftMotor and rightMotor
// once you figure out which is which. Also change names in .h file
Adafruit_DCMotor *right_wheel_motor = AFMS.getMotor(1);
Adafruit_DCMotor *left_wheel_motor = AFMS.getMotor(2);


void go_forward(uint8_t velocity)
{
    // velocity has value between 0 and 255
    
    right_wheel_motor->setSpeed(velocity);
    left_wheel_motor->setSpeed(velocity);
    
    right_wheel_motor->run(FORWARD);
    left_wheel_motor->run(FORWARD);

    Serial.println("I am going forward.");
}
