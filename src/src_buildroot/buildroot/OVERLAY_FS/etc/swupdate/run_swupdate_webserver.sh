#!/bin/sh
# /etc/swupdate/run_swupdate_webserver.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com

#TEST IF FILE LOCK EXISTS
FILEA=/tmp/SWUPDATE_WEBSERVER
if test -f "$FILEA"; then
    	echo "$FILEA exists."
    	echo "swupdate webserver already started"
    	exit 0
fi


CHANNEL=""
#TEST IF UPDATE CHANNEL FILE EXITST
FILEB=/etc/swupdate/UPDATE_CHANNEL
if test -f "$FILEB"; then
    	echo "$FILEB exists."
    	COPY="$(cat /etc/swupdate/UPDATE_CHANNEL)"
    	CHANNEL="-e stable,${COPY}"
    	echo "using update channel ${CHANNEL}"
else
	echo "$FILEB not exists. starting webserver with no channel flag"
    	exit 0
fi




echo "----  STARTING SWUPDATE WEBSERVER ----"
touch /tmp/SWUPDATE_WEBSERVER
echo "----  STARTING SWUPDATE WEBSERVER ON PORT 8081 ----"
swupdate -v -f /etc/swupdate/swupdate.cfg -k /etc/swupdate/swupdatekey_public.pem ${CHANNEL} -w "--document-root /var/www/swupdate --port 8081"
