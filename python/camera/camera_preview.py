import numpy as np
import cv2
import argparse
import time,os

frame_index = 0
frames_ps = 0
fps_start = 0
#camera_src = []

parser = argparse.ArgumentParser()
parser.add_argument('--dev', type=str, required=True,
                    help='[usb|"url" of IP camera]input video device')
args = parser.parse_args()

if args.dev == 'usb':
    print("Using onboard usb camera")
    camera_src = 0
else:    
    print("Using ip camera with url(s)", args.dev)
    camera_src = args.dev

cap = cv2.VideoCapture(camera_src)
if not cap.isOpened():
    print("video[%s] cannot be openned"%camera_src)
    raise RuntimeError("video[%s] cannot be openned"%camera_src)
        
width, height = cap.get(3), cap.get(4)
print(width, height)

cap.set(3, 640)
cap.set(4, 480)

while(True):
    fstart = time.time()
    if (frame_index == 0):
        print("first frame")
        fps_start = fstart

    elapse = fstart - fps_start
    #print("elapse:%.3f"%elapse)
    if (elapse > 1.0):
        print("FPS:[%.3f]"%(frames_ps/elapse))
        fps_start = fstart
        frames_ps = 0

    start = time.time()        
    # capture frame-by-frame
    ret, frame = cap.read()
    end = time.time()
    print("  read:%.4f"%(end-start))

    cv2.imshow('frame', frame)
    frame_index += 1
    frames_ps += 1

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
