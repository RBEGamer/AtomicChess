#!/bin/sh
# /etc/swupdate/run_swupdate_hawkbit.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com

#TEST IF FILE LOCK EXISTS
FILEA=/tmp/SWUPDATE_HAWKBIT
if test -f "$FILEA"; then
    	echo "$FILEA exists."
    	echo "swupdate hawkbit already started"
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
	echo "$FILEB not exists. starting hawkbit with no channel flag"
    	exit 0
fi


echo "----  STARTING SWUPDATE HAWKBIT ----"
touch /tmp/SWUPDATE_HAWKBIT
swupdate -v -f /etc/swupdate/swupdate.cfg ${CHANNEL} -u "${SURICATTA_ARGS}"

