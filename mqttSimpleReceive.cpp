#include "header.h"

/*
  ArduinoMqttClient - WiFi Simple Receive

  This example connects to a MQTT broker and subscribes to a single topic.
  When a message is received it prints the message to the serial monitor.

  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev.2 board

  This example code is in the public domain.
*/



///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "UniOfCam-IoT";        // your network SSID (name)
char pass[] = "cymf9Jhr";    // your network password (use for WPA, or use as key for WEP)




// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);



const char broker[] = "test.mosquitto.org";
int        port     = 1883; // non encrypted access
const char topic[]  = "IDP211"; // if both the sender and receier are connected to the same topic, data will be sent



void mqtt_Simple_receive(void)
{

  while(1)
  {
    int messageSize = mqttClient.parseMessage();
    if (messageSize) 
    {
      // we received a message, print out the topic and contents
      Serial.print("Received a message with topic '");
      Serial.print(mqttClient.messageTopic());
      Serial.print("', length ");
      Serial.print(messageSize);
      Serial.println(" bytes:");
  
      // use the Stream interface to print the contents
      while (mqttClient.available()) 
      {
        Serial.print((char)mqttClient.read()); // need to store the values later
      }
      Serial.println();
  
      Serial.println();
    }
  }
}
