#include "header.h"

void send_to_wifi(String send_message) {

  // ensuring connection is still valid
  mqttClient.poll();

  // setting topic address and sending message
  mqttClient.beginMessage(topic);
  mqttClient.print(send_message);
  mqttClient.endMessage();
}
