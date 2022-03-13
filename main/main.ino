#include "header.h"

uint8_t velocity;

uint8_t velocity_of_right_wheel;
uint8_t velocity_of_left_wheel;

bool picked_up_block = false;


// -------------------- wifi settings

char ssid[] = "UniOfCam-IoT";        // your network SSID (name)
char pass[] = "cymf9Jhr";    // your network password (use for WPA, or use as key for WEP)

String incoming; // global variables
float angle, distance;

WiFiClient wifiClient; // creating wifi client object
MqttClient mqttClient(wifiClient);

const char broker[] = "broker.hivemq.com"; // mqtt brokers
//const char broker[] = "test.mosquitto.org";
//const char broker[] = "public.mqtthq.com";

int        port     = 1883; // non encrypted access
const char topic[]  = "IDP211"; // if both the sender and receier are connected to the same topic, data will be sent

// -----------------------


void setup()
{
  AFMS.begin(50); // motor setup


  pinMode(amberLED, OUTPUT); // defining o/p LED pinmodes
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);


  amberLEDtimer = millis (); // initialising flashing LED timer


  myservo.attach(servoPin); // setting servo parameters
  pos = 0;
  myservo.write(0);


  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();


  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  mqttClient.setId("211ARDUINO");

  //might not be workingn this line
  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword("Arduino211", "CUED");



  // we are connecting to broker "test.mosquitto.org"
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }


  Serial.println("You're connected to the MQTT broker!");
  Serial.println();


  // connecting to channel/topic IDP211
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(topic);

}


void loop() {
  read_from_wifi(); // read the angle and distance

  while ((abs(angle) > 9) && (slow_mode_activate == false)) // old less precise control system which uses direct feedback
  {
    read_from_wifi();
    flashamberled();
    if (angle > 0)
    {
      turn_left(90);
    }
    else
    {
      turn_right(90);
    }
  }


  if (distance >= 0.1) // move forward if too far away
  {
    flashamberled();
    go_forward(int(255 / 1.08));
  }


  /*if (angle > 0)  // slower predictive control system
    {
    turn_left_to_angle(abs(angle), 90);
    }
    else
    {
    turn_right_to_angle(abs(angle), 90);
    }
    unsigned long time_end_turn = millis(); //can declare this at top of main
    while (millis() < time_end_turn + 1000)
    {
    //1000ms to allow camera to catch up
    }
    read_from_wifi(); // update parameters*/


  while ((abs(angle) > 3) && (slow_mode_activate == true)) // using increased accuracy when looking for block
  {
    if (angle > 0)  // checking whether the robot needs to be moved left or right
    {
      turn_left_to_angle(abs(angle), 90);
    }
    else
    {
      turn_right_to_angle(abs(angle), 90);
    }

    unsigned long time_end_turn = millis(); //including delay to allow for camera to catch up
    while (millis() < time_end_turn + 1000)
    {
      flashamberled();
    }
    read_from_wifi(); // update parameters
    flashamberled();
  }


  if ((distance < -0.1) && (slow_mode_activate == true)) { // enters slow mode and uses predictive distance movement
    move_forward_given_distance(-0.5 * distance, 125); // deliberately undercalibrated so does not overshoot
  }



  else if ((distance >= -0.1) && (slow_mode_activate == true) && (picked_up_block == true)) // in slow mode when looking to drop off the block
  {

    Serial.println("Entering dropping off code");


    while (distance < -0.01) { // create blocking loop to move forward until distance threshold is met
      move_forward_given_distance(0.005, 90);

      unsigned long moving_forward_delay = millis(); // adding in delay
      while (millis() < moving_forward_delay + 200)
      {
        read_from_wifi();
        flashamberled();
      }
    }


    servo_backward(); // releasing block

    picked_up_block = false; // resetting flag

    // reverse to clear block
    unsigned long time_to_reverse = millis();
    while (millis() < time_to_reverse + 3000)
    {
      go_backward(125);
    }

    digitalWrite(redLED, LOW); // resetting LEDs to indicate block colour
    digitalWrite(greenLED, LOW);


    //  stop and listen until a non slow distance is recieved indicating the next checkpoint
    stop_the_robot();
    while (distance <= 0)
    {
      read_from_wifi();
    }
  }

  else if ((distance >= -0.1) && (slow_mode_activate == true) && (picked_up_block == false)) // enters threshold and stops listening to CV and enters colour sensing loop
  { // picking up code

    Serial.println("Entering picking up code");

    servo_backward(); // make sure the servo is back

    while (analogRead(A0) <= 200) { // checking distance sensor on robot for block

      move_forward_given_distance(0.005, 125);
      //go_forward(90);

      unsigned long short_block_search_delay = millis(); // small delay between movements
      while (millis() < short_block_search_delay + 200)
      {
        flashamberled();
      }
    }

    move_forward_given_distance(0.015, 125);  // moving forward small amount to put block closer to colour sensor

    stop_the_robot();
    digitalWrite(amberLED, LOW);

    //Serial.println(analogRead(A1));
    if (analogRead(A1) > 140) { // checking colour sensor
      Serial.print("Colour value is: ");
      Serial.println("red");
      digitalWrite(redLED, HIGH);
      send_to_wifi("red");
    }
    if (analogRead(A1) > 40 && analogRead(A1) <= 140) {
      Serial.print("Colour value is: ");
      Serial.println("blue");
      digitalWrite(greenLED, HIGH);
      send_to_wifi("blue");
    }

    digitalWrite(amberLED, LOW);
    unsigned long flash_led_delay = millis(); // 5 second delay to ensure LED can be read
    while (millis() < flash_led_delay + 5000)
    {
      //  pure delay
    }

    toggleAmberLED(); // restart flashing LED


    // A2 is opto switch to decect if the block is picked up

    unsigned long forward_to_block_timeout = millis(); //moving forward until block is detected by
    while (analogRead(A2) < 500 && (millis() < (forward_to_block_timeout + 5000))) {

      // If I have picked up the block or if the block is stuck in the
      move_forward_given_distance(0.005, 125);

      unsigned long forward_to_block_delay = millis(); // small delay between steps
      while (millis() < forward_to_block_delay + 200)
      {
        flashamberled();
      }
    }

    if (millis() >= forward_to_block_timeout + 5000) { // timeout condition is met so must reapproach
      send_to_wifi("reapproach");

      unsigned long move_backward_timeout = millis();
      while (millis() < move_backward_timeout + 5000) // move backwards for 5 seconds
      {
        go_backward(125);
        flashamberled();
      }
    }

    if (analogRead(A2) >= 500) { // block is detected by underfloor sensor
      move_forward_given_distance(0.05, 100);
      servo_forward();
      picked_up_block = true;
    }

    // wait for the next message
    while (mqttClient.parseMessage() == 0) {
      // do nothing
    }
    read_from_wifi(); // read the message
  }
}
