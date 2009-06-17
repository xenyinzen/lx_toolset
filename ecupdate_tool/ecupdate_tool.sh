#!/bin/sh

killall fnkey
sleep 0.2
rmmod ec_scid
rmmod ec_batd
rmmod ec_ftd
rmmod ec_miscd
sleep 0.2
insmod char-1.37/ec_miscd.ko
insmod char-1.37/ec_ftd.ko
insmod char-1.37/ec_batd.ko
insmod char-1.37/ec_scid.ko
sleep 0.2

echo The file ready to write: $1.
echo "Now write, please wait by patience..."
sleep 1

./update_ecrom $1

echo "EC update finishes. Now poweroff."

sleep 2
dir=`pwd`
cd /root
#rm -rf $dir
mv $dir /tmp

poweroff
