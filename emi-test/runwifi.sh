#!/bin/sh

# FIXME
FTPSERVIP="172.16.1.50"
FILE="stonefile.img"

dd if=/dev/mem of=$FILE  bs=1024 count=100000

i=0
while [ 1 ]; do

ncftpput -u name -p password $FTPSERVIP $DIRECTORY $FILE  2>/dev/null 1>&2

sleep 2

ncftpget -u name -p password $FTPSERVIP /tmp $DIRECTORY/$FILE 1>/dev/null 2>&1

diff $FILE /tmp/$FILE

rm /tmp/$FILE

echo "${i} finished."
let i=i+1

done
