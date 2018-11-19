import numpy as np
import cv2

source1 = "rtsp://admin:adminSUOATW@192.168.1.64/Streaming/Channels/1"
source2 = "rtsp://admin:adminSUOATW@192.168.1.64/Streaming/Channels/2"

ip_camera = "http://root:nxpcesdemo@10.192.244.122/mjpg/1/video.mjpg"

cap = cv2.VideoCapture(ip_camera)
width, height = cap.get(3), cap.get(4)
print(width, height)

cap.set(3, 640)
cap.set(4, 480)

while(True):
    # capture frame-by-frame
    ret, frame = cap.read()

    # operation on the frame
    #gray = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)

    #print(frame)
    cv2.imshow('frame', frame)
    #cv2.imshow('frame', gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
