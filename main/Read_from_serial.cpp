#include "header.h"

String incomingStr;

void read_from_wifi(void) {

  while (mqttClient.available() == false) { // wait until data has been sent
    //empty loop
  }

  incomingStr = "";
  int messageSize = mqttClient.parseMessage();
  if (messageSize)
  {
    if (mqttClient.available()) {
      incomingStr = mqttClient.readString();
      //Serial.println(incomingStr);
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
}
