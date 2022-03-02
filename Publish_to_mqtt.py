from multiprocessing.connection import Client
import paho.mqtt.client as mqtt
from random import randrange, uniform
import time

mqttBroker = "test.mosquitto.org"

client = mqtt.Client("211sender")
client.username_pw_set('Arduino211', 'CUED')
client.connect(mqttBroker)

while(True):
    randAngle = uniform(-90, 90)
    randDistance = uniform(1.0, 10.0)
    sendData = str(round(randAngle,2)) + ";" + str(round(randDistance,2))
    client.publish("IDP211", sendData)
    print("Just published " + str(sendData) + " to topic IDP211")
    time.sleep(5)


