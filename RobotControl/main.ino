// final IDP competition code

// including header file with required libraries
#include "header.h"

// creating global variables
uint8_t velocity;

uint8_t velocity_of_right_wheel;
uint8_t velocity_of_left_wheel;

bool picked_up_block = false;

String incoming;
float angle, distance;


// -------------------- wifi settings

// UniOfCam-IoT wifi settings
//char ssid[] = "UniOfCam-IoT";        // your network SSID (name)
//char pass[] = "WnqmPHAA";    // your network password (use for WPA, or use as key for WEP)

// mobile hotspot settings
char ssid[] = "Kohei’s iPhone";        // your network SSID (name)
char pass[] = "--redacted--";    // your network password (use for WPA, or use as key for WEP)


// creating wifi client object
WiFiClient wifiClient; 
MqttClient mqttClient(wifiClient);

// list of potential MQTT brokers
const char broker[] = "broker.hivemq.com";
//const char broker[] = "test.mosquitto.org";
//const char broker[] = "public.mqtthq.com";

int        port     = 1883; // non encrypted access
const char topic[]  = "IDP211"; // if both the sender and receier are connected to the same topic, data will be sent

// -----------------------



// setup tab to run once
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



  // we are connecting to broker
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  // print if the MQTT client has failed to connect
  if (!mqttClient.connect(broker, port))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1); // end script
  }

  // show that client is connected
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();


  // connecting to channel/topic IDP211
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(topic);

}


// main loop
void loop() {
  read_from_wifi(); // read the angle and distance

  // non-slow mode control system which is faster but less accurate
  while ((abs(angle) > 9) && (slow_mode_activate == false))
  {
    read_from_wifi();
    if (angle > 0)  // left is +ve and right is -ve
    {
      turn_left_to_angle(abs(angle), 200);
    }
    else
    {
      turn_right_to_angle(abs(angle), 200);
    }

    // including delay for after turn has been completed
    unsigned long time_end_turn = millis();
    while (millis() < time_end_turn + 1000)
    {
      //1000ms to allow camera to catch up
    }
  }

  // move forward if too far away
  if (distance >= 0.1)
  {
    flashamberled();
    go_forward(int(255 / 1.08));
  }



  // trying to get accurately close to the block with the 0.1 threshold
  while (slow_mode_activate == true)
  {

    // code for controlling robot in slow mode if the block has not been picked up
    if (!picked_up_block && ((abs(angle) > 3) || (distance < -0.1)))
    {

      Serial.println("I am trying to get accurately close to the block with the 0.1 threshold ");

      if ((abs(angle) > 3)) // using increased accuracy when looking for block
      {
        if (angle > 0)  // checking whether the robot needs to be moved left or right
        {
          turn_left_to_angle(abs(angle), 150);
        }
        else
        {
          turn_right_to_angle(abs(angle), 150);
        }

        // including delay to allow for camera to catch up
        unsigned long time_end_turn = millis(); 
        while (millis() < time_end_turn + 1500)
        {
          // update parameters during delay
          flashamberled();
          read_from_wifi();
        }
      }


      // robot moves forward if the angle condition is met
      else if ((distance < -0.1))
      {
        // multiply by negative as slow nmode is only true when distances are negative
        move_forward_given_distance(-0.8 * distance, 140); // deliberately undercalibrated so does not overshoot
      }
    }


    // code for controlling robot in slow mode if the block has been picked up
    else if (picked_up_block && ((abs(angle) > 3) || (distance < -0.07))) // -0.03
    {
      // enter this if i have just rolled down the ramp and want to turn

      if ((abs(angle) > 3) && (distance < -0.07)) // using increased accuracy when looking to place the block
      {
        if (angle > 0)  // checking whether the robot needs to be moved left or right
        {
          turn_left_to_angle(abs(angle), 175); // takes rougly 300-1000ms
        }
        else
        {
          turn_right_to_angle(abs(angle), 175);
        }

        //including delay to allow for camera to catch up
        unsigned long time_end_turn = millis(); 
        while (millis() < time_end_turn + 1500)
        {
          // update parameters during delay
          flashamberled();
          read_from_wifi();
        }
      }


      else if (distance < -0.07) // -0.03
      {
        // enter this if i have just rolled down ramp and the angle is correct
        // enters slow mode and uses predictive distance movement
        move_forward_given_distance(-0.8 * distance, 175); // deliberately undercalibrated so does not overshoot
      }


      else // enter this if I am close enough to drop off zone and need to correct the angle
      {
        break;
      }

    }
    else
    {
      // print for debugging
      Serial.println("broken out of slowmode loop");
      Serial.print("Picked up block value: ");
      Serial.println(picked_up_block);
      break;
    }
  }
  // once I exit this while loop, angle is below 3 degrees and ditance is greater or equal than -0.1



  // start of picking up code
  if ((distance >= -0.1) && (slow_mode_activate == true) && (picked_up_block == false)) // enters threshold and stops listening to CV and enters colour sensing loop
  {
    
    Serial.println("Entering picking up code");

    servo_backward(); // make sure the servo is back

    while (analogRead(A0) <= 200)
    { // checking distance sensor in front of robot for block

      // while block is not detected keep moving robot forward by small increments
      move_forward_given_distance(0.005, 125);

      // small delay between movements
      unsigned long short_block_search_delay = millis();
      while (millis() < short_block_search_delay + 100)
      {
        flashamberled();
        // robot is not listening to wifi while in the picking up code
      }
    }

    // moving forward small amount to put block closer to colour sensor
    move_forward_given_distance(0.025, 125);

    // stop the robot to read from the colour sensor
    stop_the_robot();
    digitalWrite(amberLED, LOW);

    // ---------- caluculating average of the colour sensor value
    int colour_sensor_value = 0;
    for (int i = 0; i < 5; i++)
    {
        colour_sensor_value += analogRead(A1);
    }

    colour_sensor_value /= 5;

    // ----------
    
    // checking if the colour is red or blue using arduino analogRead with 125 == 610mV threshold
    if (colour_sensor_value >= 125) // used to say analogRead(A1) instead of colour_sensor_value
    {
      // checking colour sensor
      Serial.print("Colour value is: ");
      Serial.println("red");
      digitalWrite(redLED, HIGH);
      send_to_wifi("red"); // tell CV colour of block
    }

    else if (colour_sensor_value < 125) // used to say analogRead(A1) instead of colour_sensor_value;
    {
      Serial.print("Colour value is: ");
      Serial.println("blue");
      digitalWrite(greenLED, HIGH);
      send_to_wifi("blue"); // tell CV colour of block
    }


    // 5 second delay to ensure LED can be read
    unsigned long flash_led_delay = millis();
    while (millis() < flash_led_delay + 5000)
    {
      //  pure delay to allow robot information to catch up with camera
    }

    toggleAmberLED(); // restart flashing LED


    // A2 is opto switch to placed under robot to decect if the block is picked up
    
    // creating timeout for it block becomes stuck and does not trigger sensor
    unsigned long forward_to_block_timeout = millis(); 
    while (analogRead(A2) < 500 && (millis() < (forward_to_block_timeout + 8000)))
    {
      // moving forward by small increment
      move_forward_given_distance(0.005, 175);

      // small delay between steps
      unsigned long forward_to_block_delay = millis();
      while (millis() < forward_to_block_delay + 100)
      {
        flashamberled();
      }
    }

    if (millis() >= forward_to_block_timeout + 8000)
    {
      // timeout condition is met so must reapproach

      unsigned long move_backward_timeout = millis();
      while (millis() < move_backward_timeout + 5000) // move backwards for 5 seconds
      {
        go_backward(255);
        flashamberled();
      }

      send_to_wifi("reapproach"); // tell CV side to resend location of the block and try again
    }

    // block is detected by underfloor sensor
    if (analogRead(A2) >= 500) {
      move_forward_given_distance(0.05, 100); // move forward small amount to get block in optimum pick up position
      servo_forward(); // move the servo
      send_to_wifi("pickedup"); // tell the CV that block has been picked up
      picked_up_block = true; // setting flag
    }

    // wait for the next message
    stop_the_robot();

    // wait for next non-slow mode message from CV
    while (distance <= 0) 
    {
      read_from_wifi();
    }
  }
  // END OF PICKING UP CODE



  // START OF DROPPING OFF BLOCK CODE
  else if ((distance >= -0.07) && (slow_mode_activate == true) && (picked_up_block == true)) // in slow mode when looking to drop off the block
  {
    
    Serial.println("Entering dropping off code");

    // create blocking loop to move forward until distance threshold is met
    while (distance < -0.02) {

      // moving forward by small increment
      move_forward_given_distance(0.005, 90);

      // small delay between movements
      unsigned long moving_forward_delay = millis();
      while (millis() < moving_forward_delay + 100)
      {
        read_from_wifi(); // update parameters
        flashamberled();
      }
    }

    // move forward by small distance to account for threshold
    move_forward_given_distance(0.02, 90);

    servo_backward(); // releasing block

    picked_up_block = false; // resetting flag


    // reverse to clear block
    unsigned long time_to_reverse = millis();
    while (millis() < time_to_reverse + 2000)
    {
      go_backward(255);
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
}
