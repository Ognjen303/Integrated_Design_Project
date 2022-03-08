#include "header.h"

const long send_interval = 1000;
unsigned long previousSendMillis = 0;

int send_count = 0;

void send_to_wifi(String send_message) {

  Serial.println(topic);
  
  mqttClient.poll();

  mqttClient.beginMessage(topic);
  mqttClient.print(send_message);
  mqttClient.endMessage();
  /*unsigned long currentSendMillis = millis();

    if (currentSendMillis - previousSendMillis >= send_interval)
      {
        // save the last time a message was sent
        previousSendMillis = currentSendMillis;

        Serial.print("Sending message to topic: ");
        Serial.println(topic);

        //Serial.println(message);

        // send message, the Print interface can be used to set the message contents
        mqttClient.beginMessage(topic);

        mqttClient.print("hello ");
        mqttClient.print(count);


        mqttClient.print(message);
        mqttClient.endMessage();

        Serial.println();

        count++;
      }*/
}
