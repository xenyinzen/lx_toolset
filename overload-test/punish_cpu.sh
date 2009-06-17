#!/bin/bash

cd alsa-lib-1.0.9

./configure
while [ 1 == 1 ]; do

make
make clean

done
