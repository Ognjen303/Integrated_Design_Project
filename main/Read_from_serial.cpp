#include "header.h"

String incomingStr;

void read_from_wifi(void) {
  incomingStr = "";

 if (mqttClient.available() > 0) {
    incomingStr.concat((char)mqttClient.read());
    Serial.println(".");
  }

  for (int i = 0; i <= incomingStr.length(); i++) {
    if (incomingStr.substring(i, i + 1) == ";") {
      angle = incomingStr.substring(0, i).toFloat();
      distance = incomingStr.substring(i + 1, incomingStr.length()).toFloat();
        Serial.println(angle);
        Serial.println(distance);
      break;
    }
  }
}
