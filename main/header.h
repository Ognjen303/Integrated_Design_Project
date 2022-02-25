#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include "stdlib.h"


// -----Here you define the digital led pin numbers
// -----double check you where you pluged in the led cables on arduino

#define amber_LED 8
#define red_LED  9
#define green_LED  10





extern Adafruit_MotorShield AFMS;
extern Adafruit_DCMotor *right_wheel_motor;
extern Adafruit_DCMotor *left_wheel_motor;




void go_forward(uint8_t velocity);
void go_backward(uint8_t velocity);
void turn_right(uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel);
void turn_left(uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel);
void test_go_forward_and_back(uint8_t velocity);
unsigned int read_integer_input(void);
