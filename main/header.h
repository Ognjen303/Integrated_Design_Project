#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include "stdlib.h"


// -----Here you define the digital led pin numbers
// -----double check you where you pluged in the led cables on arduino

// D8 for amber, D9 for red, D10 for green led
#define amberLED 8
#define redLED  9
#define greenLED  10





extern Adafruit_MotorShield AFMS;
extern Adafruit_DCMotor *right_wheel_motor;
extern Adafruit_DCMotor *left_wheel_motor; 


extern const unsigned long amberLEDinterval;
extern const unsigned long redLEDinterval;
extern const unsigned long greenLEDinterval;

extern unsigned long amberLEDtimer;
extern unsigned long redLEDtimer;
extern unsigned long greenLEDtimer;






void go_forward(uint8_t velocity);
void go_backward(uint8_t velocity);
void turn_right(uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel);
void turn_left(uint8_t velocity_of_right_wheel, uint8_t velocity_of_left_wheel);
void test_go_forward_and_back(uint8_t velocity);
unsigned int read_integer_input(void);
void toggleAmberLED (void);
void toggleRedLED (void);
void toggleGreenLED (void);
void flashamberled(void);
void DetectColour(void);
