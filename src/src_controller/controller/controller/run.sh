#!/bin/bash


FILEA=/tmp/ATC/ATC_LOCK
touch "$FILEA"


echo 'ATCCONTROLLER_VIRT_1.0' > /tmp/VERSION
echo 'HWREV_VIRT' > /tmp/hwrevision
echo 'DOCKER' > /tmp/BOOTPART


while true; do
	if test -f "$FILEA"; then
		./atc_controller -writeconfig -hwvirtual -autoplayer
		sleep 10
	else
	    	exit 1
	fi
done