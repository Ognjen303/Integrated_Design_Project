#include "header.h"


const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

void mqtt_Simple_sender(String message)
{
    while(1)
    {
      // call poll() regularly to allow the library to send MQTT keep alives which
      // avoids being disconnected by the broker
      mqttClient.poll();
    
      // avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
      // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
      unsigned long currentMillis = millis();
      
      if (currentMillis - previousMillis >= interval)
      {
        // save the last time a message was sent
        previousMillis = currentMillis;
    
        Serial.print("Sending message to topic: ");
        Serial.println(topic);
        /*
        Serial.println("hello ");
        Serial.println(count);
        */
  
        //Serial.println(message);      
        
        // send message, the Print interface can be used to set the message contents
        mqttClient.beginMessage(topic);
        /*
        mqttClient.print("hello ");
        mqttClient.print(count);
        */

        mqttClient.print(message);
        mqttClient.endMessage();
    
        Serial.println();
    
        count++;
      }
      
      
    }  
}
