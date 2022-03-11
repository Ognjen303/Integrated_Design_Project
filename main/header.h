#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include "stdlib.h"
#include "Servo.h"

#include <ArduinoMqttClient.h>

#include <WiFiNINA.h>


 
// -----Here you define the digital led pin numbers
// -----double check you where you pluged in the led cables on arduino

// D5 for amber, D6 for red, D7 for green led
#define amberLED 5
#define redLED  6
#define greenLED  7

extern Adafruit_MotorShield AFMS;
extern Adafruit_DCMotor *right_wheel_motor;
extern Adafruit_DCMotor *left_wheel_motor; 


extern Servo myservo;

extern const uint8_t servoPin;
extern unsigned long servo_timer;
extern int pos;
extern const unsigned long servo_forward_interval;
extern const unsigned long servo_backward_interval;
extern const uint16_t final_angle;
extern uint16_t distance_sensorValue;
extern uint16_t colour_sensorValue;



// used by helens test code
extern const unsigned long amberLEDinterval;

extern unsigned long amberLEDtimer;

//------------------------------

extern uint8_t old_velocity;


extern bool i_am_going_forward;
extern bool i_am_going_backward;
extern bool i_am_turning_left;
extern bool i_am_turning_right;
extern bool i_am_detecting_colour;
extern bool i_am_detecting_red_colour;
extern bool i_am_detecting_blue_colour;
extern bool i_stopped;
extern bool sensor_status;
extern bool colour_detector;

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
extern bool slow_mode_activate, picked_up_block;

void stop_the_robot(void);
void go_forward(uint8_t velocity);
void go_backward(uint8_t velocity);
void turn_right(uint8_t right_velocity);
void turn_left(uint8_t left_velocity);
void test_go_forward_and_back(uint8_t velocity);
unsigned int read_integer_input(void);
void toggleAmberLED (void);
void flashamberled(void);
void ToggleDetectingSystem(void);
void ToggleColourSensor(void);
void DetectColour(void);
void servo_rotating(void);
void test_drive_in_a_square(uint8_t velocity);
void reset_all_flags(void);
void mqtt_Simple_receive(void);
void mqtt_Simple_sender(String message);
void read_from_wifi(void);
void send_to_wifi(String send_message);
void turn_left_to_angle(float angle, uint8_t left_velocity);
void turn_right_to_angle(float angle, uint8_t right_velocity);
void move_forward_given_distance(float forward_distance, uint8_t forward_velocity);
void servo_forward (void);
void servo_backward (void);
