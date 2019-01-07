# encoding: utf-8
import paho.mqtt.client as mqtt

HOST = "10.192.244.140"
PORT = 1883

TOPIC = chat

def on_connect(client, userdata, flags, rc):
    print("Connected with result code" + str(rc))
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    print(msg.topic + " " + ":" + str(msg.payload))

def test():
    client = mqtt.Client()

    client.on_connect = on_connect
    client.on_message = on_message
    
    client.connect(HOST, PORT, 60)
    #client.publish("chat", "hello liefyuan", 2)

    client.loop_forever()

if __name__ == '__main__':
    test()  
