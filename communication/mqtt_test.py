import paho.mqtt.client as mqtt 
from random import randrange, uniform
import time

mqttBroker = "mqtt.eclipseprojects.io" 

client = mqtt.Client("Python")
client.username_pw_set('IDP211', 'CUED')
client.connect(mqttBroker) 

while True:
    randNumber = uniform(20.0, 21.0)
    client.publish("IDP211", randNumber)
    print("Just published " + str(randNumber) + " to topic IDP211")
    time.sleep(1)
    