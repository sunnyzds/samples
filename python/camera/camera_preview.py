# ref: http://ex2tron.top

import numpy as np
import cv2

cap = cv2.VideoCapture(0)

width, height = cap.get(3), cap.get(4)

print(width, height)

cap.set(3, 640)
cap.set(4, 480)

#capture.set(cv2.CAP_PROP_FRAME_WIDTH, width * 2)
#capture.set(cv2.CAP_PROP_FRAME_HEIGHT, height * 2)

while(True):
    # capture frame-by-frame
    ret, frame = cap.read()

    # operation on the frame
    #gray = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)
    #cv2.imshow('frame', gray)
    cv2.imshow('frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
