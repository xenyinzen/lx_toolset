#!/bin/bash

echo
echo "====================="
echo "| START TO TEST ... |"
echo "====================="
echo

# to create a big file on your disk
dd if=/dev/mem of=./bigfile bs=1024 count=102400
# delay 2 seconds
sleep 2

./punish_cpu.sh &

./punish_harddisk.sh &

./punish_netcard.sh &

./punish_USB.sh &

./punish_video.sh &




