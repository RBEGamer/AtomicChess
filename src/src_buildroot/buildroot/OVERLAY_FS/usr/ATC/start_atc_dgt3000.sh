#! /bin/sh
# /usr/ATC/start_atc_dgt3000.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
#
# atc        Starts the ATC ATC_DGT3000_EXT in an endless loop
#

if test -f "/usr/ATC/atc_dgt3000"; then
    touch /usr/ATC/usr/ATC/atc_dgt3000
else
    exit 1
fi

while true; do
	if test -f "/usr/ATC/ATC_DGT3000_EXT"; then
        echo "STARTING ATC ATC_DGT3000_EXT"
		/usr/ATC/atc_dgt3000
		sleep 10
	else
	    	exit 1
	fi	
done
