import numpy as np
import cv2

#ip_camera = "http://root:nxpcesdemo@10.192.244.122/mjpg/1/video.mjpg"
ip_camera= "rtsp://admin:Volansys@123@192.168.1.65:88/videoMain"

#cap = cv2.VideoCapture(ip_camera)
cap = cv2.VideoCapture(0)
width, height = cap.get(3), cap.get(4)
print(width, height)

cap.set(3, 640)
cap.set(4, 480)

frame_index = 0

def get_frame():
    if frame_index > 1000:
        frame_index = 0
    inPathName = "./test_image/frame_%d.jpg"%frame_index
    img = cv2.imread(inPathName)
    return img
    
while(True):    
    frame = get_frame()
    cv2.imshow('frame', gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
