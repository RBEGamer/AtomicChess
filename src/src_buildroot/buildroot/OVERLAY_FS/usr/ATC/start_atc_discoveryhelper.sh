#! /bin/sh
cd "$(dirname "$0")"
# /usr/ATC/start_atc_discoveryhelper.sh ATCProject Marcel Ochsendorf marcelochsendorf.com
while true; do
	FILEA=/usr/ATC/ATC_LOCK_DCH
	if test -f "$FILEA"; then
    	echo "STARTING ATC DISCOVERY HELPER"
		#/usr/ATC/atc_discovery_helper 
		/usr/ATC/atc_discovery_helper --cfgfolderpath /usr/ATC/atc_config/ || true
		sleep 10
	else
	    	exit 1
	fi	
done
