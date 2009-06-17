#!/bin/sh

base=`date +"%s"`
current=0
while [ 1 ]; do
#	echo -n "5H"
	clear
	cat ./5H.txt
	current=`date +"%s"`
	time=`expr $current - $base`
	h=`expr $time / 3600`
	m0=`expr  $time % 3600`
	m=`expr $m0 / 60`
	s=`expr $time % 60`
	echo "				Have been running for $h:$m:$s"
	sleep  1
done
