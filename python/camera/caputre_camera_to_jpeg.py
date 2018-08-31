import os
import sys
import argparse

import cv2
print(cv2.__version__)

def extracImages(pathIn, pathOut):
    if pathIn == "usb":
        cap = cv2.VideoCapture(0)
    else:
        cap = cv2.VideoCapture(pathIn)
    if not cap.isOpened():
        print("video[%s] cannot be openned"%pathIn)
        raise RuntimeError("video[%s] cannot be openned"%pathIn)

    cap.set(3, 640)
    cap.set(4, 480)

    ret = True
    frame_index = 0
    while(ret):
        # read one frame
        ret, frame = cap.read()
        #print("Read %d frame "%frame_index, ret)
        # show the frame
        cv2.imshow('frame', frame)

        #write to file
        if cv2.waitKey(1) == ord('c'):
            print("capture frame")
            outPathName = pathOut+"//frame_%d.jpg"%frame_index
            print("outPathName:%s"%outPathName)
            cv2.imwrite(outPathName, frame)
            frame_index += 1

        if cv2.waitKey(1) == ord('q'):
            break

if __name__ == "__main__":
    print("Start...")
    argparser = argparse.ArgumentParser()
    argparser.add_argument("--pathIn", help="path to video")
    argparser.add_argument("--pathOut", help="path to save video")
    args = argparser.parse_args()
    #print("args:"+args)
    print(args)
    if not os.path.exists(args.pathOut):
        print("%s is not exist"%args.pathOut)
        os.makedirs(args.pathOut)
    extracImages(args.pathIn, args.pathOut)
