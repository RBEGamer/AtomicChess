#!/bin/sh

echo "current startup partiation"
CURRBOOTPART=$(cat /proc/cmdline | sed -e 's/^.*root=//' -e 's/ .*$//')
echo "$CURRBOOTPART"

# CHECK IF SCRIPT WAS ALREADY EXECUTED
FILEA=/tmp/PARTITION_SWITCHED
if test -f "$FILEA"; then
    	echo "$FILEA exists."
    	echo "partitions already switched"
    	exit 0
fi


echo "$CURRBOOTPART" > /etc/swupdate/BOOTPART

if [ "$CURRBOOTPART" = "/dev/mmcblk0p2" ]; then
               echo "$CURRBOOTPART IS PARTITION A"
               echo "B" > /etc/swupdate/UPDATE_CHANNEL
               echo "/dev/mmcblk0p3" > /etc/swupdate/NEXT_BOOTPART
               echo "NEXT PARTITON TO BOOT IS IS PARTITION B /dev/mmcblk0p3"
fi

if [ "$CURRBOOTPART" = "/dev/mmcblk0p3" ]; then
               echo "$CURRBOOTPART IS PARTITION B"
               echo "A" > /etc/swupdate/UPDATE_CHANNEL
               echo "/dev/mmcblk0p2" > /etc/swupdate/NEXT_BOOTPART
               echo "NEXT PARTITON TO BOOT IS IS PARTITION A /dev/mmcblk0p2"
fi



touch /tmp/PARTITION_SWITCHED
