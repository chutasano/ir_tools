#!/usr/bin/python3

# adapted from www.cs.uml.edu/~cgao

import sys                                 
import ssl
import json
import paho.mqtt.client as mqtt
import subprocess as s

#change these based on your setup
muser="user"
mpass="user"
murl="ec2-18-218-182-153.us-east-2.computer.amazonaws.com"
mport=8883 # port number as an int

#called while client tries to establish connection with the server 
def on_connect(mqttc, obj, flags, rc):
    if rc==0:
        print ("Subscriber Connection status code: "+str(rc)+" | Connection status: successful")
        mqttc.subscribe("ircode", qos=0)
    elif rc==1:
        print ("Subscriber Connection status code: "+str(rc)+" | Connection status: Connection refused")

#called when a topic is successfully subscribed to
def on_subscribe(mqttc, obj, mid, granted_qos):
    print("Subscribed: "+str(mid)+" "+str(granted_qos)+"data"+str(obj))

#called when a message is received by a topic
def on_message(mqttc, obj, msg):
    print("Received message from topic: "+msg.topic+" | QoS: "+str(msg.qos)+" | Data Received: "+str(msg.payload))
    if (msg.topic == "ircode"):
        s.call(["./sender", msg.payload])
        print ("sent")

#creating a client with client-id=mqtt-test
mqttc = mqtt.Client(client_id="pi")
mqttc.username_pw_set(username=muser,password=mpass)

mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
mqttc.on_message = on_message
mqttc.tls_set(ca_certs="/home/pi/key/ca.crt")

#connecting to aws-account-specific-iot-endpoint
mqttc.connect(murl, port=mport) #AWS IoT service hostname and portno

#automatically handles reconnecting
mqttc.loop_forever()
