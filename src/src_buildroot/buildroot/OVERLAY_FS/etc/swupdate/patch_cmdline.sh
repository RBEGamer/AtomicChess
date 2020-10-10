#!/bin/sh

echo "current startup partiation"
CURRBOOTPART=$(cat /proc/cmdline | sed -e 's/^.*root=//' -e 's/ .*$//')
echo "$CURRBOOTPART"

# CHECK IF SCRIPT WAS ALREADY EXECUTED
FILEA=/boot/cmdline.txt
if test -f "$FILEA"; then
    	echo "$FILEA exists."
    	echo "/boot/cmdline.txt exists"
else
	echo "--- /boot/cmdline.txt DOES NOT EXISTS ---"
	exit 0
fi


# CHECK IF NEXT_BOOT_EXISTS
# THIS FILE CONTAINS THE PATH OF THE OTHER PARTITON
# WHICH HAS TO BOOT ON THE NEXT START
FILEB=/NEXT_BOOTPART
if test -f "$FILEA"; then
    	echo "$FILEA exists."
    	echo "/NEXT_BOOTPART exists"
else
	echo "--- /NEXT_BOOTPART DOES NOT EXISTS ---"
	exit 0
fi

# NOW PATCH THE CMDLINE.TXT IN THE BOOT PARTITON



touch /tmp/PARTITION_SWITCHED
