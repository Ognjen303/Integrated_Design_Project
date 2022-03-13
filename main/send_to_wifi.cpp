#include "header.h"

const long send_interval = 1000;
unsigned long previousSendMillis = 0;

int send_count = 0;

void send_to_wifi(String send_message) {

  //Serial.println(topic);
  
  mqttClient.poll();

  mqttClient.beginMessage(topic);
  mqttClient.print(send_message);
  mqttClient.endMessage();
}
