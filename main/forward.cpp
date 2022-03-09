#include "header.h"


Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// rename later muMotor_1 and _2 to leftMotor and rightMotor
// once you figure out which is which. Also change names in .h file
Adafruit_DCMotor *right_wheel_motor = AFMS.getMotor(1);
Adafruit_DCMotor *left_wheel_motor = AFMS.getMotor(2);


uint8_t old_velocity = 0;

// could be later on usefull for debuging
// i_am_going_forward can be set to true only and the end of the else statement
// please do not set it as true anywhere else
bool i_am_going_forward = false; 


void go_forward(uint8_t velocity)
{
    if (i_am_going_forward)
      return;

    else
    {
      reset_all_flags();
  
      // velocity has value between 0 and 255
      
      right_wheel_motor->setSpeed(velocity);
      left_wheel_motor->setSpeed(velocity);
      
      right_wheel_motor->run(FORWARD);
      left_wheel_motor->run(FORWARD);

      i_am_going_forward = true;

      // old_velocity = velocity;
    }
}
