#! /bin/sh
# /usr/ATC/start_atc_controller.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
#
# atc        Starts the ATC Controller in an endless loop
#
sleep 10
while true; do
	FILEA=/usr/ATC/ATC_LOCK_CONTROLLER
	if test -f "$FILEA"; then
    	echo "STARTING ATC CONTROLLER"
		/usr/ATC/atc_controller -cfgfolderpath /usr/ATC/atc_config/
		sleep 10
	else
	    	exit 1
	fi	
done
