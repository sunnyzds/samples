#!/bin/bash

if [ $1'x' == 'x' ];then
    echo "kmssink"
    gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,width=640,height=480 ! kmssink
fi

if [ $1'x' == '1x' ];then
    echo "autovideosink"
    gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,width=640,height=480 ! autovideosink
fi

if [ $1'x' == '2x' ];then
    echo "waylandsink"
    gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw,width=640,height=480 ! waylandsink
fi

