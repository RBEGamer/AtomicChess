#!/bin/sh
# /etc/swupdate/run_swupdate_webserver.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
FILEA=/tmp/SWUPDATE_WEBSERVER
if test -f "$FILEA"; then
    	echo "$FILEA exists."
    	echo "swupdate webserver already started"
else
	echo "----  STARTING SWUPDATE HAWKBIT ----"
	touch /tmp/SWUPDATE_WEBSERVER
	echo "----  STARTING SWUPDATE WEBSERVER ON PORT 8081 ----"
	swupdate -v -f /etc/swupdate/swupdate.cfg -e B -k /etc/swupdate/swupdatekey_public.pem -w "--document-root /var/www/swupdate --port 8081"
fi

