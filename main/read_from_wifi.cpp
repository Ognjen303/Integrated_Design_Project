#include "header.h"

// define local variables
String incomingStr;
bool slow_mode_activate;

//begin function
void read_from_wifi(void) {

  incomingStr = "";
  int messageSize = mqttClient.parseMessage();

  // if message is available
  if (messageSize)
  {
    if (mqttClient.available()) {
      // store the recieved string
      incomingStr = mqttClient.readString();
      Serial.println(incomingStr); // print incoming string for debugging

      // killing the robot code
      if (incomingStr == "stop") {
        move_forward_given_distance(0.1, 255);
        stop_the_robot();
        while (1) {
          // kill the robot in this loop
          Serial.println("I have stopped");
        }
      }
    }

    // itterating through incoming string to split into angle and distance
    for (int i = 0; i <= incomingStr.length(); i++) {
      if (incomingStr.substring(i, i + 1) == ";") { // ; used to separate angle and distance in string
        angle = incomingStr.substring(0, i).toFloat();
        distance = incomingStr.substring(i + 1, incomingStr.length()).toFloat();

        // setting slow-mode flag
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
