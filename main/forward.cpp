#include "header.h"


Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// rename later muMotor_1 and _2 to leftMotor and rightMotor
// once you figure out which is which. Also change names in .h file
Adafruit_DCMotor *right_wheel_motor = AFMS.getMotor(2);
Adafruit_DCMotor *left_wheel_motor = AFMS.getMotor(1);


uint8_t old_velocity = 0;

// could be later on usefull for debuging
// i_am_going_forward can be set to true only and the end of the else statement
// please do not set it as true anywhere else

bool i_am_going_forward = false; 


void go_forward(uint8_t velocity)
{
      //reset_all_flags();
      right_wheel_motor->setSpeed(1.0*velocity);
      left_wheel_motor->setSpeed(1.08*velocity);
      
      right_wheel_motor->run(FORWARD);
      left_wheel_motor->run(FORWARD);

    /*
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
    }*/
}

void move_forward_given_distance(float forward_distance, uint8_t forward_velocity) {
  unsigned long start_forward_move = millis(); // record time that the turning is started
  
  //Serial.println(1000*(rotate_angle*6.0/90.0));

  //Serial.println(int(1000 * (forward_distance * 19)));
  while (millis() - start_forward_move < int(1000 * (forward_distance * 19))) { // 19 s/metre gives the time to move the given distance
    go_forward(forward_velocity); // turning the robot left
    read_from_wifi();
    flashamberled();
  }
  
  //Serial.println("exiting left turn");
  stop_the_robot(); // stop turning
}
