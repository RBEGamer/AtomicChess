#! /bin/sh
# /usr/ATC/start_atc_gui.sh ATCProject Marcel Ochsendorf marcelochsendorf.com
while true; do
	FILEA=/usr/ATC/ATC_LOCK_SRV
	if test -f "$FILEA"; then
    	echo "STARTING ATC SERVER"
		#/usr/ATC/atc_server 
		/usr/ATC/atc_server || true
		sleep 10
	else
	    	exit 1
	fi	
done
