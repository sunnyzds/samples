import os
import sys
import argparse

import cv2
print(cv2.__version__)

import cv2
import numpy as np

width = 640
height = 480
radius = 140

if __name__ == "__main__":
    print("Start...")
    image = np.zeros((height, width))
    print("w:%d h:%d"%(image.shape[1], image.shape[0]))

    cv2.circle(image, (width/2-1, height/2-1), radius, (255, 255, 255), -1)
    
    cv2.imshow('frame', image)
    cv2.imwrite("black_hole_640_480.png", image)
    
    ret = True
    while(ret):    
        if cv2.waitKey(1) == ord('q'):
            break
