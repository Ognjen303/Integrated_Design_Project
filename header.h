#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include "stdlib.h"
#include "Servo.h"

#include <ArduinoMqttClient.h>


#if defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#endif

  


// -----Here you define the digital led pin numbers
// -----double check you where you pluged in the led cables on arduino

// D8 for amber, D9 for red, D10 for green led
#define amberLED 8
#define redLED  9
#define greenLED  10







extern Adafruit_MotorShield AFMS;
extern Adafruit_DCMotor *right_wheel_motor;
extern Adafruit_DCMotor *left_wheel_motor; 


extern Servo myservo;



// used by helens test code
extern const unsigned long amberLEDinterval;
extern const unsigned long redLEDinterval;
extern const unsigned long greenLEDinterval;

extern unsigned long amberLEDtimer;
extern unsigned long redLEDtimer;
extern unsigned long greenLEDtimer;

//------------------------------


extern const uint8_t servoPin;




extern uint8_t old_velocity;


extern bool i_am_going_forward;
extern bool i_am_going_backward;
extern bool i_am_turning_left;
extern bool i_am_turning_right;
extern bool i_am_detecting_colour;
extern bool i_am_detecting_red_colour ;
extern bool i_am_detecting_blue_colour ;
extern bool i_stopped;




// ----------------------
// files used in mqttSimpleReceive
extern char ssid[];
extern char pass[];


extern WiFiClient wifiClient;
extern MqttClient mqttClient;

extern const char broker[];
extern int        port; // non encrypted access
extern const char topic[]; // if both the sender and receier are connected to the same topic, data will be sent

extern float angle, distance;


void stop_the_robot(void);
void go_forward(uint8_t velocity);
void go_backward(uint8_t velocity);
void turn_right(void);
void turn_left(void);
void test_go_forward_and_back(uint8_t velocity);
unsigned int read_integer_input(void);
void toggleAmberLED (void);
void toggleRedLED (void);
void toggleGreenLED (void);
void flashamberled(void);
void DetectColour(void);
void servo_rotating(void);
void test_drive_in_a_square(uint8_t velocity);
void reset_all_flags(void);
void mqtt_Simple_receive(void);
