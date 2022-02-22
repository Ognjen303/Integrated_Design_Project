#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

extern Adafruit_MotorShield AFMS;
extern Adafruit_DCMotor *right_wheel_motor;
extern Adafruit_DCMotor *left_wheel_motor;



void go_forward(uint8_t velocity);
void go_backward(uint8_t velocity);
void turn_right(uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel);
void turn_left(uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel);
