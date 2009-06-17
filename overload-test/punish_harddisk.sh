#!/bin/bash

i=0
dd if=/dev/mem of=bigfile bs=1024 count=200000

while [ 1 == 1 ]; do

let i=i+1
cp ./bigfile /tmp/bigfile1
cmp ./bigfile /tmp/bigfile1
diff ./bigfile /tmp/bigfile1

echo "${i} finished."

rm /tmp/bigfile1

done
