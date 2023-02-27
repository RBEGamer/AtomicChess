#! /bin/sh
# /usr/ATC/start_atc_dgt3000.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
#
# atc        Starts the ATC ATC_DGT3000_EXT in an endless loop
#

if test -f "/usr/ATC/ATC_DGT3000_EXT"; then
    touch /usr/ATC/ATC_DGT3000_EXT
else
    exit 1
fi

while true; do
	if test -f "/usr/ATC/ATC_DGT3000_EXT"; then
        echo "STARTING ATC ATC_DGT3000_EXT"
		/usr/ATC/ATC_DGT3000_EXT
		sleep 10
	else
	    	exit 1
	fi	
done
