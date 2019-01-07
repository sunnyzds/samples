# encoding: utf-8
import paho.mqtt.client as mqtt

HOST = "10.192.244.140"
PORT = 1883

def test():
    client = mqtt.Client()
    client.connect(HOST, PORT, 60)
    client.publish("chat", "hello liefyuan", 2)

    client.loop_forever()

if __name__ == '__main__':
    test()  