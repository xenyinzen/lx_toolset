#!/bin/sh
#
# memtest.sh
#
# Shell script to help isolate memory failures under linux
#
# Author: Doug Ledford  + contributors
#
# (C) Copyright 2000-2002 Doug Ledford; Red Hat, Inc.
# This shell script is released under the terms of the GNU General
# Public License Version 2, June 1991.  If you do not have a copy
# of the GNU General Public License Version 2, then one may be
# retrieved from http://people.redhat.com/dledford/GPL.html
#
# Note, this needs bash2 for the wait command support.
# This is where we will run the tests at
if [ -z "$TEST_DIR" ]; then
  TEST_DIR=/burn-tmp
fi

# The location of the linux kernel source file we will be using
if [ -z "$SOURCE_FILE" ]; then
  SOURCE_FILE=$TEST_DIR/linux.tar.gz
fi

if [ ! -f "$SOURCE_FILE" ]; then
  echo "Missing source file $SOURCE_FILE"
  exit 1
fi

# How many passes to run of this test, higher numbers are better
if [ -z "$NR_PASSES" ]; then
  NR_PASSES=2000000000
fi

# Guess how many megs the unpacked archive is
if [ -z "$MEG_PER_COPY" ]; then
  #MEG_PER_COPY=$(ls -l $SOURCE_FILE | awk '{print int($5/1024/1024) * 4}')
  MEG_PER_COPY=1
fi

# How many trees do we have to unpack in order to make our trees be larger
# than physical RAM?  If we don't unpack more data than memory can hold
# before we start to run the diff program on the trees then we won't
# actually flush the data to disk and force the system to reread the data
# from disk.  Instead, the system will do everything in RAM.  That doesn't
# work (as far as the memory test is concerned).  It's the simultaneous
# unpacking of data in memory and the read/writes to hard disk via DMA that
# breaks the memory subsystem in most cases.  Doing everything in RAM without
# causing disk I/O will pass bad memory far more often than when you add
# in the disk I/O.
if [ -z "$NR_SIMULTANEOUS" ]; then
#  NR_SIMULTANEOUS=$(free | awk -v meg_per_copy=$MEG_PER_COPY 'NR == 2 {print int($2*1.5/1024/meg_per_copy + (($2/1024)%meg_per_copy >= (meg_per_copy/2)) + (($2/1024/32) < 1))}')
  NR_SIMULTANEOUS=1
fi

# Should we unpack/diff the $NR_SIMULTANEOUS trees in series or in parallel?
if [ ! -z "$PARALLEL" ]; then
  PARALLEL="yes"
else
  PARALLEL="no"
fi

if [ ! -z "$JUST_INFO" ]; then
  echo "TEST_DIR:		$TEST_DIR"
  echo "SOURCE_FILE:		$SOURCE_FILE"
  echo "NR_PASSES:		$NR_PASSES"
  echo "MEG_PER_COPY:		$MEG_PER_COPY"
  echo "NR_SIMULTANEOUS:	$NR_SIMULTANEOUS"
  echo "PARALLEL:		$PARALLEL"
  echo
  exit
fi

cd $TEST_DIR

# Remove any possible left over directories from a cancelled previous run
rm -fr linux linux.orig linux.pass.*

# Unpack the one copy of the source tree that we will be comparing against
#tar -xzf $SOURCE_FILE
gunzip -c $SOURCE_FILE | tar -xf -
mv linux linux.orig

i=0
while [ "$i" -lt "$NR_PASSES" ]; do
  j=0
  while [ "$j" -lt "$NR_SIMULTANEOUS" ]; do
    if [ $PARALLEL = "yes" ]; then
      (mkdir $j; gunzip -c $SOURCE_FILE | tar -xf -C $j -; mv $j/linux linux.pass.$j; rmdir $j) &
    else
      #tar -xzf $SOURCE_FILE
      gunzip -c $SOURCE_FILE | tar -xf -
      mv linux linux.pass.$j
    fi
    #j=`expr $j + 1`
    j=$(($j + 1))
  done
  wait
  j=0
  while [ "$j" -lt "$NR_SIMULTANEOUS" ]; do
    if [ $PARALLEL = "yes" ]; then
      (diff -U 3 -rN linux.orig linux.pass.$j; rm -fr linux.pass.$j) &
    else
      diff -U 3 -rN linux.orig linux.pass.$j > /dev/null
      if [ $? == 0 ]; then
	      run_ok=$run_ok
      else
	      run_ok=0
	      touch /tmp/frt-burn-error
	      exit 1
      fi
      rm -fr linux.pass.$j
    fi
    #j=`expr $j + 1`
    j=$(($j + 1))
  done
  wait
  #i=`expr $i + 1`
  i=$(($i + 1))
done

# Clean up after ourselves
rm -fr linux linux.orig linux.pass.*

exit 0
