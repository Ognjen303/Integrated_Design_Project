#include "header.h"


uint8_t velocity;
uint8_t velocity_of_right_wheel;
uint8_t velocity_of_left_wheel;


unsigned int mode = 0;

bool end_program = false;



void setup()
{
    AFMS.begin();
    pinMode(amber_LED, OUTPUT);

    
    Serial.begin(9600);
    Serial.println("Choose mode in which to run robot.");
    Serial.println("TESTS:");
    Serial.println("Press 1 for go_forward_and_back");
    Serial.println("Press 2 to test Amber LED.");
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

             // Amber light should light up 
             digitalWrite(amber_LED, HIGH);
             
             // test here some other stuff
             
             break;

          case 3:

             Serial.println("What is the velocity you wish to go at in test case 2?");
             Serial.println("Input a integer between 0 and 255, where 255 is max velocity.");
             
             velocity = read_integer_input();

             Serial.println("Your input is: ");
             Serial.println(velocity);
             
             go_forward(velocity);
             delay(4000);
             go_backward(velocity);
             delay(4000);
             right_wheel_motor->run(RELEASE);
             left_wheel_motor->run(RELEASE);
             
             end_program = true;
             break;
          
          default:
             Serial.println("You messed up the input somehow :( ");
              
             end_program = true;
             break;
       }
      
    mode = 0;                // clear the mode for reuse
    

    if(end_program)
    {
        while(1);
    }
}
