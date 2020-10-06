#! /bin/sh
while true; do
	FILEA=/usr/ATC/ATC_LOCK
	if test -f "$FILEA"; then
    		echo "STARTING ATC GUI"
		/usr/ATC/atc_ui
		sleep 10
	else
	    	exit 1
	fi	
done
