#!/bin/bash

if [ $1'x' == 'x' ];then
	python camera_preview.py --dev usb
else
	python camera_preview.py --dev http://root:nxpcesdemo@10.192.244.122/mjpg/1/video.mjpg
fi
