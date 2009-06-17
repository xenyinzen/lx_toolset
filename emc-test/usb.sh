#!/bin/bash

USBDEV=$1
[ -d "$USBDEV" ] || mkdir $USBDEV
df | grep /dev/$USBDEV > /dev/null || mount /dev/$USBDEV $USBDEV


TMPFILE="$1.tmp"

echo "begin test usbdisk. will write data to usbdisk"

while [ 1 ]; do
	dd if=/dev/$USBDEV of=${USBDEV}/${TMPFILE} bs=1k count=$2 1>/dev/null 2>&1
	sync
	sleep 1;
	cat ${USBDEV}/${TMPFILE} > /dev/null
	rm ${USBDEV}/${TMPFILE}
done
echo "finished."
