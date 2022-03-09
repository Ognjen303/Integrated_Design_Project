/*#include "header.h"

  String incomingStr;


  void read_from_wifi(void) {

  incomingStr = "";

    while (mqttClient.parseMessage()) {
    //Serial.println("empty loop");
  }

  int messageSize = mqttClient.parseMessage();
  if (messageSize)
  {
    if (mqttClient.available()) {
      incomingStr = mqttClient.readString();
      Serial.println(incomingStr);
    }
    for (int i = 0; i <= incomingStr.length(); i++) {
      if (incomingStr.substring(i, i + 1) == ";") {
        angle = incomingStr.substring(0, i).toFloat();
        distance = incomingStr.substring(i + 1, incomingStr.length()).toFloat();

        //Serial.println(angle);
        //break;
      }
    }
  }
  }*/

#include "header.h"

String incomingStr;
bool slow_mode_activate;

void read_from_wifi(void) {

  incomingStr = "";
  int messageSize = mqttClient.parseMessage();

  if (messageSize)
  {
    if (mqttClient.available()) {
      incomingStr = mqttClient.readString();
      Serial.println(incomingStr);
    }
    for (int i = 0; i <= incomingStr.length(); i++) {
      if (incomingStr.substring(i, i + 1) == ";") {
        angle = incomingStr.substring(0, i).toFloat();
        distance = incomingStr.substring(i + 1, incomingStr.length()).toFloat();
        //Serial.println(angle);
        //break;
      }
      if (distance < 0) {
        slow_mode_activate = true;
        //delay(1500);
        //Serial.println("looking for block");
      }
      else {
        slow_mode_activate = false;
      }
    }
  }
}
