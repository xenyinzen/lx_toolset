#!/bin/bash

HD="/dev/sda"
TMPFILE="hd.tmp"
BS=1k

if [ $# -ne 1 ]; then
	echo "usage: $0 speed"
	exit 0
fi

while [ 1 ]; do
	dd if=$HD of=$TMPFILE bs=$BS count=$1 1>/dev/null 2>&1
	if [ $? -eq 0 ]; then
		echo "write ok"
	fi
	sync
	sleep 1
done

rm ${TMPFILE}

echo "finished."
