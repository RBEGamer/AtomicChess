#! /bin/sh
# /usr/ATC/start_atc_gui.sh ATCProject Marcel Ochsendorf marcelochsendorf.com
while true; do
	FILEA=/usr/ATC/ATC_LOCK_GENSIO_SKR
	if test -f "$FILEA"; then
    		echo "STARTING GENSIO"
		    #gensiot -i serialdev,/dev/ttySKR,9600N81 -a telnet,tcp,7002
		    gensiot -i serialdev,/dev/ttySKR,9600N81 -a tcp,7002

	else
	    	exit 1
	fi	
done
