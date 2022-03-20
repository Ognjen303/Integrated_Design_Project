#include "header.h"


Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// rename later muMotor_1 and _2 to leftMotor and rightMotor
// once you figure out which is which. Also change names in .h file
Adafruit_DCMotor *right_wheel_motor = AFMS.getMotor(2);
Adafruit_DCMotor *left_wheel_motor = AFMS.getMotor(1);




void go_forward(uint8_t velocity)
{
      // left wheel needs to spin slightly faster to account for drift
      right_wheel_motor->setSpeed(1.0*velocity);
      left_wheel_motor->setSpeed(1.08*velocity);
      
      right_wheel_motor->run(FORWARD);
      left_wheel_motor->run(FORWARD);
}




void move_forward_given_distance(float forward_distance, uint8_t forward_velocity) {
  
  unsigned long start_forward_move = millis(); // record time that the turning is started
  while (millis() - start_forward_move < int(1000 * (forward_distance * 11))) { // 11 s/metre gives the time to move the given distance
    go_forward(forward_velocity); // turning the robot left
    read_from_wifi();
    flashamberled();
  }
  stop_the_robot(); // stop turning
}
