#! /bin/sh
# /usr/ATC/start_atc_webui.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
while true; do
	FILEA=/usr/ATC/ATC_LOCK
	if test -f "$FILEA"; then
    		echo "STARTING ATC WEBGUI"
		/usr/ATC/atc_ui -platform webgl:port=1337
		sleep 10
	else
	    	exit 1
	fi	
done
