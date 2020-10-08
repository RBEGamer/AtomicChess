#! /bin/sh
# /usr/ATC/start_atc_gui.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
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
