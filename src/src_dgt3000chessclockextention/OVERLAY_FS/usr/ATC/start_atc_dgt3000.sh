#! /bin/sh
# /usr/ATC/start_atc_controller.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
#
# atc        Starts the ATC Controller in an endless loop
#
while true; do
	FILEA=/usr/ATC/ATC_DGT3000_EXT
	if test -f "$FILEA"; then
    		echo "STARTING ATC ATC_DGT3000_EXT"
		/usr/ATC/ATC_DGT3000_EXT
		sleep 10
	else
	    	exit 1
	fi	
done
