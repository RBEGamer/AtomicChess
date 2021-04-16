#! /bin/sh
# /usr/ATC/start_atc_controller.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
#
# atc        Starts the ATC Controller in an endless loop
#
while true; do
	FILEA=/usr/ATC/ATC_LOCK
	if test -f "$FILEA"; then
    		echo "STARTING ATC CONTROLLER"
		/usr/ATC/atc_controller
		sleep 10
	else
	    	exit 1
	fi	
done
