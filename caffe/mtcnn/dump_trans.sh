#!/bin/bash
CMD=$1

if [ ${CMD}'X' == 'dumpX' ]; then
    echo "dump"
    python ../caffe_dump.py --proto det1_upgrade.prototxt --model det1_upgrade.caffemodel --dump 1
fi

if [ ${CMD}'X' == 'transX' ]; then
    echo "trans"
    python ../caffe_dump.py --proto det1_upgrade.prototxt --model det1_upgrade.caffemodel --trans 1
fi
