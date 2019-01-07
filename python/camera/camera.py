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

while(True):
    # capture frame-by-frame
    if not cap.isOpened():
        raise RuntimeError('Could not start camera')
    ret, frame = cap.read()
    outPathName = "./test_image/frame_%d.jpg"%frame_index
    print("capture frame:%s"%(outPathName))

    cv2.imwrite(outPathName, frame)

    frame_index += 1

    #cv2.imshow('frame', gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    if frame_index > 1000:
        break

cap.release()
