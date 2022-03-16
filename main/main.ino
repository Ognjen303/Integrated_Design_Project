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
    /*read_from_wifi();
      flashamberled();
      if (angle > 0)
      {
      turn_left(120);
      }
      else
      {
      turn_right(120);
      }*/
    read_from_wifi();
    if (angle > 0)  // slower predictive control system
    {
      turn_left_to_angle(abs(angle), 255);
    }
    else
    {
      turn_right_to_angle(abs(angle), 255);
    }
    unsigned long time_end_turn = millis(); //can declare this at top of main
    while (millis() < time_end_turn + 1000)
    {
      //1000ms to allow camera to catch up
    }
    read_from_wifi(); // update parameters
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


  // trying to get accurately close to the block with the 0.1 threshold
  while (slow_mode_activate == true)
  {

    // CODE FOR CONTROLING THE ROBOT IN SLOW_MODE IF I HAVENT PICKED UP THE BLOCK
    if (!picked_up_block && ((abs(angle) > 3) || (distance < -0.1)))
    {

      Serial.println("I am trying to get accurately close to the block with the 0.1 threshold ");

      if ((abs(angle) > 3)) // using increased accuracy when looking for block
      {
        if (angle > 0)  // checking whether the robot needs to be moved left or right
        {
          turn_left_to_angle(abs(angle), 120);
        }
        else
        {
          turn_right_to_angle(abs(angle), 120);
        }

        unsigned long time_end_turn = millis(); //including delay to allow for camera to catch up
        while (millis() < time_end_turn + 2000)
        {
          flashamberled();
        }
        read_from_wifi(); // update parameters
        flashamberled();

      }


      else if ((distance < -0.1))
      { // enters slow mode and uses predictive distance movement
        move_forward_given_distance(-0.6 * distance, 140); // deliberately undercalibrated so does not overshoot
      }
    }


    // CODE FOR CONTROLING THE ROBOT IN SLOW_MODE IF I HAVE PICKED UP THE BLOCK
    else if (picked_up_block && ((abs(angle) > 3) || (distance < -0.07))) // -0.03
    {
      // enter this if i have just rolled down the ramp and want to turn

      if ((abs(angle) > 3) && (distance < -0.07)) // using increased accuracy when looking for block
      {
        if (angle > 0)  // checking whether the robot needs to be moved left or right
        {
          turn_left_to_angle(abs(angle), 200); // takes rougly 300-1000ms
        }
        else
        {
          turn_right_to_angle(abs(angle), 200);
        }

        unsigned long time_end_turn = millis(); //including delay to allow for camera to catch up
        while (millis() < time_end_turn + 3000)
        {
          flashamberled();
          read_from_wifi();
        }
        read_from_wifi(); // update parameters
        flashamberled();
      }


      else if (distance < -0.07) // -0.03
      {
        // enter this if i have just rolled down ramp and the angle is correct
        // enters slow mode and uses predictive distance movement
        move_forward_given_distance(-0.8 * distance, 125); // deliberately undercalibrated so does not overshoot
      }


      else // enter this if I am close enough to drop off zone and need to correct the angle
      {
        break;
      }

    }
    else
    {
      Serial.println("broken out of slowmode loop");
      Serial.print("Picked up block value: ");
      Serial.println(picked_up_block);
      break;
    }
  }
  // once I exit this while loop, angle is below 3 degrees and ditance is greater or equal than -0.1



  // START OF PICK UP CODE
  if ((distance >= -0.1) && (slow_mode_activate == true) && (picked_up_block == false)) // enters threshold and stops listening to CV and enters colour sensing loop
  {
    // picking up code

    Serial.println("Entering picking up code");

    servo_backward(); // make sure the servo is back

    while (analogRead(A0) <= 200)
    { // checking distance sensor on robot for block

      move_forward_given_distance(0.005, 125);
      //go_forward(90);

      unsigned long short_block_search_delay = millis(); // small delay between movements
      while (millis() < short_block_search_delay + 100)
      {
        flashamberled();
      }
    }

    move_forward_given_distance(0.01, 125);  // moving forward small amount to put block closer to colour sensor

    stop_the_robot();
    digitalWrite(amberLED, LOW);

    //Serial.println(analogRead(A1));

    // caluculating average of the colour sensor value
    int colour_sensor_value = 0;
    for (int i = 0; i < 5; i++)
    {
        colour_sensor_value += analogRead(A1);
    }

    colour_sensor_value /= 5;
    
    
    if (colour_sensor_value > 100) // used to say analogRead(A1) instead of colour_sensor_value
    {
      // checking colour sensor
      Serial.print("Colour value is: ");
      Serial.println("red");
      digitalWrite(redLED, HIGH);
      send_to_wifi("red");
    }

    else if (colour_sensor_value > 40 && colour_sensor_value <= 100) // used to say analogRead(A1) instead of colour_sensor_value;
    {
      Serial.print("Colour value is: ");
      Serial.println("blue");
      digitalWrite(greenLED, HIGH);
      send_to_wifi("blue");
    }



    digitalWrite(amberLED, LOW);
    unsigned long flash_led_delay = millis(); // 5 second delay to ensure LED can be read
    while (millis() < flash_led_delay + 5000)
    {
      //  pure delay to allow robot information to catch up with camera
    }

    toggleAmberLED(); // restart flashing LED


    // A2 is opto switch to decect if the block is picked up

    unsigned long forward_to_block_timeout = millis(); //moving forward until block is detected by
    while (analogRead(A2) < 500 && (millis() < (forward_to_block_timeout + 8000)))
    {
      // If I have picked up the block or if the block is stuck in the
      move_forward_given_distance(0.005, 175);

      unsigned long forward_to_block_delay = millis(); // small delay between steps
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

      send_to_wifi("reapproach");
    }

    if (analogRead(A2) >= 500) { // block is detected by underfloor sensor
      move_forward_given_distance(0.05, 100);
      servo_forward();
      send_to_wifi("pickedup");
      picked_up_block = true;
    }

    // wait for the next message
    stop_the_robot();
    /*while (mqttClient.parseMessage() == 0) {
      // do nothing
      }
      read_from_wifi(); // read the message*/

    while (distance <= 0) // wait for next non-slow mode input
    {
      read_from_wifi();
    }
  }
  // END OF PICKING UP CODE



  // START OF DROPPING OFF BLOCK CODE
  else if ((distance >= -0.07) && (slow_mode_activate == true) && (picked_up_block == true)) // in slow mode when looking to drop off the block
  {
    // needs editing for greater accuracy
    Serial.println("Entering dropping off code");


    while (distance < -0.02) { // create blocking loop to move forward until distance threshold is met
      Serial.println("entering edging forward loop");
      move_forward_given_distance(0.005, 90);

      unsigned long moving_forward_delay = millis(); // adding in delay
      while (millis() < moving_forward_delay + 100)
      {
        read_from_wifi();
        flashamberled();
      }
    }

    move_forward_given_distance(0.04, 90);
    //turn_right_to_angle(20, 90);


    servo_backward(); // releasing block

    picked_up_block = false; // resetting flag

    // reverse to clear block
    unsigned long time_to_reverse = millis();
    while (millis() < time_to_reverse + 2500)
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
