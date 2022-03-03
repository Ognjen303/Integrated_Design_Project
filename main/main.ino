#include "header.h"


uint8_t velocity;

uint8_t velocity_of_right_wheel;
uint8_t velocity_of_left_wheel;


unsigned int mode = 0;

bool end_program = false;



void setup()
{
    AFMS.begin();
    pinMode(amberLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    amberLEDtimer = millis ();
    redLEDtimer = millis ();
    greenLEDtimer = millis ();
    
  
    myservo.attach(servoPin);
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

        
      switch(mode)
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
             while (1)
             {
               flashamberled();
               DetectColour();
             }

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

             servo_rotating();
             
             Serial.println("I am rotating servo.");

             end_program = true;

             break;

          
          default:
             Serial.println("You messed up the input somehow :( ");
              
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
             
       }
      
    mode = 0;                // clear the mode for reuse
    

    if(end_program)
    {
        while(1);
    }
}
