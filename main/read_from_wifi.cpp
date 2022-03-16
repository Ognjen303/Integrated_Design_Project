#include "header.h"

String incomingStr;
bool slow_mode_activate;

void read_from_wifi(void) {

  incomingStr = "";
  int messageSize = mqttClient.parseMessage();

  //Serial.println("reading");

  if (messageSize)
  {
    if (mqttClient.available()) {
      incomingStr = mqttClient.readString();
      Serial.println(incomingStr);
      if (incomingStr == "stop") {
        move_forward_given_distance(0.1, 175);
        stop_the_robot();
        while (1) {
          // kill the robot in this loop
          Serial.println("I have stopped");
        }
      }
    }
    for (int i = 0; i <= incomingStr.length(); i++) {
      if (incomingStr.substring(i, i + 1) == ";") {
        angle = incomingStr.substring(0, i).toFloat();
        distance = incomingStr.substring(i + 1, incomingStr.length()).toFloat();
        //Serial.println(angle);
        //break;
        if (distance < 0) {
          slow_mode_activate = true;
        }
        else {
          slow_mode_activate = false;
        }
      }
    }
  }
}
