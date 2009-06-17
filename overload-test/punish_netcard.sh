#!/bin/bash

i=0
while [ 1 == 1 ]; do

let i=i+1
# FIXME: the following IP shoud be changed to adapt your evironment 
scp ./bigfile  root@172.16.1.50:/tftpboot/

echo "${i} finished."

done
