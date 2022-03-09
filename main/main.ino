#include "header.h"


uint8_t velocity;

uint8_t velocity_of_right_wheel;
uint8_t velocity_of_left_wheel;


unsigned int mode = 0;

bool end_program = false;
bool picked_up_block = false;


void setup()
{
  AFMS.begin(50);
  pinMode(amberLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  amberLEDtimer = millis ();
  sensor_status = true;
  colour_detector = false;


  myservo.attach(servoPin);
  pos = 0;
  myservo.write(0);
  final_angle = 130;



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

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic);
  Serial.println("-------------------------------------");







  Serial.println("Choose mode in which to run robot.");
  Serial.println("TESTS:");
  Serial.println("Press 1 for go_forward_and_back");
  Serial.println("Press 2 for Helen's test.");
  Serial.println("Press 3 to go forward for 3 seconds and stop.");
  Serial.println("Press 4 to just go forward.");
  Serial.println("Press 5 to rotate servo hand.");
  Serial.println("Press 6 to drive in a square.");
  Serial.println("Press 7 to receive messages from Ioan/Adhi via topic");
  Serial.println("Press 8 to start driving!");
  Serial.println("Press 9 to send message via topic to Adhi.");
  Serial.println("Press 10 to turn indefinitely.");
  Serial.println("Press 11 to enter new and improved control system.");
  Serial.println("Press 12 to send message to wifi.");
  Serial.println("Press 13 to enter trial colour sensing code.");

  //velocity = 150;
  //velocity_of_right_wheel = 150;
  //velocity_of_left_wheel = 150;

}

void loop()
{
  // Serial.println(Serial.available());

  mode = read_integer_input();

  Serial.println("Your input is: ");
  Serial.println(mode);


  switch (mode)
  {
    case 1:
      Serial.println("What is the velocity you wish to go at in test case 1?");
      Serial.println("Input a integer between 0 and 255, where 255 is max velocity.");


      velocity = read_integer_input();

      Serial.println("Your input is: ");
      Serial.println(velocity);

      test_go_forward_and_back(velocity);

      end_program = true;
      break;

    case 2:

      Serial.println("I am inside test case 2 from Helen");
      // sensor testing
      go_forward(100);
      ToggleDetectingSystem();
      ToggleColourSensor();
      DetectColour();

      end_program = true;
      // test here some other stuff

      break;

    case 3:

      Serial.println("What is the velocity you wish to go at in test case 3?");
      Serial.println("Input a integer between 0 and 255, where 255 is max velocity.");

      velocity = read_integer_input();

      go_forward(velocity);
      delay(3000);
      stop_the_robot();
      Serial.println("I am going forward:");
      Serial.println(i_am_going_forward);
      Serial.println("I have stopped:");
      Serial.println(i_stopped);

      end_program = true;
      break;

    case 4:

      Serial.println("What is the velocity you wish to go at in test case 4?");
      Serial.println("Input a integer between 0 and 255, where 255 is max velocity.");

      velocity = read_integer_input();

      go_forward(velocity);

      end_program = true;

      break;

    case 5:

      //servo_rotating();

      Serial.println("I am rotating servo.");

      end_program = true;

      break;

    case 6:

      Serial.println("What is the velocity you wish to go at in test case 6?");
      Serial.println("Input a integer between 0 and 255, where 255 is max velocity.");


      velocity = read_integer_input();

      Serial.println("Your input is: ");
      Serial.println(velocity);

      test_drive_in_a_square(velocity);

      end_program = true;
      break;

    case 7:
      mqtt_Simple_receive();

      end_program = true;
      break;

    case 8:
      while (1)
      {
        read_from_wifi();
        while (abs(angle) > 9)
        {
          read_from_wifi();
          if (angle > 0)
          {
            turn_left(90);
          }
          else
          {
            turn_right(90);
          }
        }
        if (distance >= 0.1)
        {
          go_forward(255);
        }
      }

      end_program = true;
      break;

    case 9:
      mqtt_Simple_sender("1");

      end_program = true;
      break;

    case 10:
      while (1) {
        turn_left(read_integer_input());
      }

      end_program = true;
      break;

    case 11:
      while (1)
      {
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
          go_forward(255);
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


        while ((abs(angle) > 4) && (slow_mode_activate == true)) // using increased accuracy when looking for block
        {
          if (angle > 0)  // checking whether the robot needs to be moved left or right
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
          read_from_wifi(); // update parameters
          flashamberled();
        }


        if ((distance < -0.1) && (slow_mode_activate == true)) { // enters slow mode and uses predictive distance movement
          move_forward_given_distance(-1 * distance, 125);
        }


        if ((distance >= -0.1) && (slow_mode_activate == true) && (picked_up_block == false)) // enters threshold and stops listening to CV and enters colour sensing loop
        { // picking up code

          servo_backward();

          while (analogRead(A0) <= 200) { // distance mesurement
            move_forward_given_distance(0.005, 125);
            //go_forward(90);
            unsigned long short_block_search_delay = millis(); //can declare this at top of main
            while (millis() < short_block_search_delay + 200)
            {
              flashamberled();
            }
            Serial.print("Distance value is: ");
            Serial.println(analogRead(A0));
          }

          stop_the_robot();
          digitalWrite(amberLED, LOW);

          Serial.println(analogRead(A1));
          if (analogRead(A1) > 140) { // checking colour sensor
            Serial.print("Colour value is: ");
            Serial.println("red");
            digitalWrite(redLED, HIGH);
            send_to_wifi("red");
          }
          if (analogRead(A1) > 40 && analogRead(A1) < 100) {
            Serial.print("Colour value is: ");
            Serial.println("blue");
            digitalWrite(greenLED, HIGH);
            send_to_wifi("blue");
          }

          unsigned long flash_led_delay = millis(); //can declare this at top of main
          while (millis() < flash_led_delay + 5000)
          {
            digitalWrite(amberLED, LOW);
          }

          toggleAmberLED(); // restart flashing LED

          move_forward_given_distance(0.1, 125);

          servo_forward();

          // set boolean
          picked_up_block = true;
        }


        if ((distance >= -0.1) && (slow_mode_activate == true) && (picked_up_block == true)) // in slow mode when looking to drop off the block
        {
          // move forward estimated distance
          // trigger servo to drop off

          move_forward_given_distance(0.1, 125);
          servo_backward();

          // set boolean
          picked_up_block = false;

          // reverse to clear block
          unsigned long time_to_reverse = millis(); //can declare this at top of main
          while (millis() < time_to_reverse + 3000)
          {
            go_backward(125);
          }

          digitalWrite(redLED, LOW);
          digitalWrite(greenLED, LOW);
        }

      }


      end_program = true;
      break;

    case 12:
      while (1) {
        send_to_wifi("hello there");
        delay(5000);
      }

      end_program = true;
      break;

    case 13:

          servo_backward();

          while (analogRead(A0) <= 200) { // distance mesurement
            move_forward_given_distance(0.005, 125);
            //go_forward(90);
            
            unsigned long short_block_search_delay = millis(); //can declare this at top of main
            while (millis() < short_block_search_delay + 200)
            {
              flashamberled();
            }
            Serial.print("Distance value is: ");
            Serial.println(analogRead(A0));
          }

          stop_the_robot();
          digitalWrite(amberLED, LOW);

          Serial.println(analogRead(A1));
          if (analogRead(A1) > 140) { // checking colour sensor
            Serial.print("Colour value is: ");
            Serial.println("red");
            digitalWrite(redLED, HIGH);
            send_to_wifi("red");
          }
          if (analogRead(A1) > 40 && analogRead(A1) < 100) {
            Serial.print("Colour value is: ");
            Serial.println("blue");
            digitalWrite(greenLED, HIGH);
            send_to_wifi("blue");
          }

          unsigned long flash_led_delay = millis(); //can declare this at top of main
          while (millis() < flash_led_delay + 5000)
          {
            digitalWrite(amberLED, LOW);
          }

          toggleAmberLED(); // restart flashing LED

          move_forward_given_distance(0.1, 125);

          servo_forward();

      end_program = true;
      break;

    default:
      Serial.println("You messed up the input somehow :( ");

      end_program = true;
      break;

  }

  mode = 0;                // clear the mode for reuse


  if (end_program)
  {
    while (1);
  }
}
