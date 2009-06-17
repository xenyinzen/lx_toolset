#!/bin/bash

USBDEV='/media/sda1'

i=0
while [ 1 == 1 ]; do

let i=i+1
cp ./bigfile ${USBDEV}/bigfile1
cmp ./bigfile ${USBDEV}/bigfile1
diff ./bigfile ${USBDEV}/bigfile1

echo "${i} finished."

done
