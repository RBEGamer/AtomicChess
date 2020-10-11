#!/bin/sh

echo "patching cmdline.tyt with the next start partition using content of file /etc/swupdate/NEXT_BOOTPART"

# CHECK IF SCRIPT WAS ALREADY EXECUTED
FILEA=/boot/cmdline.txt
if test -f "$FILEA"; then
    	echo "/boot/cmdline.txt exists"
else
	echo "--- /boot/cmdline.txt DOES NOT EXISTS ---"
	exit 0
fi


# CHECK IF NEXT_BOOT_EXISTS
# THIS FILE CONTAINS THE PATH OF THE OTHER PARTITON
# WHICH HAS TO BOOT ON THE NEXT START
FILEB=/etc/swupdate/NEXT_BOOTPART
if test -f "$FILEA"; then

    	echo "etc/swupdate/NEXT_BOOTPART exists"
else
	echo "--- /NEXT_BOOTPART DOES NOT EXISTS ---"
	exit 0
fi


# CHECK IF NEXT_BOOT_EXISTS
# THIS FILE CONTAINS THE PATH OF THE OTHER PARTITON
# WHICH HAS TO BOOT ON THE NEXT START
FILEC=/tmp/CMDLINE_PATCHED
if test -f "$FILEC"; then

        echo "/tmp/CMDLINE_PATCHED exists"
	exit 0
fi




# NOW PATCH THE CMDLINE.TXT IN THE BOOT PARTITON
CURRPB=$(cat /proc/cmdline | sed -e 's/^.*root=//' -e 's/ .*$//')
#CURRBP="$(cat /etc/swupdate/BOOTPART)"
NEXTBP="$(cat /etc/swupdate/NEXT_BOOTPART)"

echo "${CURRBP} -> ${NEXTBP} "

cat /boot/cmdline.txt
sed -i 's|'"$CURRBP"'|'"$NEXTBP"'|g' /boot/cmdline.txt
cat /boot/cmdline.txt

touch /tmp/CMDLINE_PATCHED

