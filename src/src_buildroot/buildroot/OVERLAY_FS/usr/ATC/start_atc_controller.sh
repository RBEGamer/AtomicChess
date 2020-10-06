#! /bin/sh
while true; do
	FILEA=/usr/ATC/ATC_LOCK
	if test -f "$FILEA"; then
    		#echo "STARTING ATC CONTROLLER"
		#/usr/ATC/atc_controller
		sleep 10
	else
	    	exit 1
	fi	
done
