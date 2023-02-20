#! /bin/sh
# /usr/ATC/start_atc_gui.sh ATCProject Marcel Ochsendorf marcelochsendorf.com
while true; do
	FILEA=/usr/ATC/ATC_LOCK_GUI
	if test -f "$FILEA"; then
    	echo "STARTING ATC GUI"
		#/usr/ATC/atc_ui --platform directfb
		/usr/ATC/atc_ui -platform linuxfb:fb=/dev/fb0
		sleep 10
	else
	    	exit 1
	fi	
done
