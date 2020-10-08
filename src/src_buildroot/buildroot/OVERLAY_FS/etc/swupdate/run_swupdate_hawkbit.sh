#!/bin/sh
# /etc/swupdate/run_swupdate_hawkbit.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com


#CHECK IF AN swupdate.cfg already exists
FILEA=/tmp/SWUPDATE_HAWKBIT
if test -f "$FILEA"; then
    	echo "$FILEA exists."
    	echo "swupdate hawkbit already started"
else
	echo "----  STARTING SWUPDATE HAWKBIT ----"
	touch /tmp/SWUPDATE_HAWKBIT
	swupdate -v -f /etc/swupdate/swupdate.cfg -u "${SURICATTA_ARGS}"
fi
